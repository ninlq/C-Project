#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "../CONFIG/config.h"
#include "../CLIENT_ORCHESTRE/client_orchestre.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../SERVICE/service.h"

#define MAX_SERVICES 10



static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <fichier config>\n", exeName);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char * argv[])
{
    if (argc != 2)
        usage(argv[0], "nombre paramètres incorrect");
    
    bool fin = false;

    // lecture du fichier de configuration
    config_init(argv[1]);

    // Pour la communication avec les clients
    // - création de 2 tubes nommés pour converser avec les clients
    mkfifo(PIPE_O2C, 0666);
    mkfifo(PIPE_C2O, 0666);
    // - création d'un sémaphore pour que deux clients ne
    //   ne communiquent pas en même temps avec l'orchestre
    int semaphore_key = ftok("somefile", 'A'); 
    int semaphore_id = semget(semaphore_key, 1, IPC_CREAT | 0666);
    semctl(semaphore_id, 0, SETVAL, 1);
    int pipe_orchestre_to_service[2];
    int service_semaphore_ids[MAX_SERVICES];
    
    // lancement des services, avec pour chaque service :
    for (int i = 0; i < config_getNbServices(); i++) {
        // - création d'un tube anonyme pour converser (orchestre vers service)
        
        pipe(pipe_orchestre_to_service);
        // - un sémaphore pour que le service prévienne l'orchestre de la
        //   fin d'un traitement
        int service_semaphore_key = ftok("somefile", 'B' + i);
        service_semaphore_ids[i] = semget(service_semaphore_key, 1, IPC_CREAT | 0666);
        semctl(service_semaphore_ids[i], 0, SETVAL, 0);
        // - création de deux tubes nommés (pour chaque service) pour les
        //   communications entre les clients et les services
        char service_to_client_pipe[256];
        char client_to_service_pipe[256];
        snprintf(service_to_client_pipe, sizeof(service_to_client_pipe), "service_to_client_%d", i);
        snprintf(client_to_service_pipe, sizeof(client_to_service_pipe), "client_to_service_%d", i);
        mkfifo(service_to_client_pipe, 0666);
        mkfifo(client_to_service_pipe, 0666);

        // lancement du service
        if (fork() == 0) {
            execlp("./service", "./service", i, service_semaphore_key, pipe_orchestre_to_service[0], service_to_client_pipe, client_to_service_pipe, (char *)NULL);
            exit(0);
        }
    }
    

    while (! fin)
    {
        // ouverture ici des tubes nommés avec un client
        int client_pipe = open(PIPE_C2O, O_RDONLY);
        char request[256];
        read(client_pipe, request, sizeof(request));
        // attente d'une demande de service du client
        int service_number;
        sscanf(request, "%d", &service_number);

        // détecter la fin des traitements lancés précédemment via
        // les sémaphores dédiés (attention on n'attend pas la
        // fin des traitement, on note juste ceux qui sont finis)
        for (int i = 0; i < config_getNbServices(); i++) {
            struct sembuf sem_op;
            sem_op.sem_num = 0;
            sem_op.sem_op = 0; 
            sem_op.sem_flg = IPC_NOWAIT; 
            if (semop(service_semaphore_ids[i], &sem_op, 1) == -1) {
                printf("Service %d has completed its processing.\n", i);
            }
        }

        // analyse de la demande du client
        // si ordre de fin
        if (service_number == -1) {
            write(client_pipe, "1", 1); // Envoi un code d'acceptation au client (via le tube nommé)
            fin = true; // Marquer la fin de la boucle
        }
        else if (!config_isServiceOpen(service_number)) {
            write(client_pipe, "-1", 2); // Envoi un code d'erreur au client
        }
        // sinon si service non ouvert
        else {
            // Envoi d'un code d'acceptation
            write(client_pipe, "1", 1);
            // Génération d'un mot de passe
            char password[256];
            snprintf(password, sizeof(password), "password_%d", service_number);
            // Envoi du code de travail au service
            write(pipe_orchestre_to_service[1], &service_number, sizeof(service_number));
            // Envoi du mot de passe au service
            write (pipe_orchestre_to_service[1], password, sizeof(password));

            // Attente de la fin du traitement via le sémaphore
	    for (int i = 0; i < config_getNbServices(); i++) {
               struct sembuf sem_op;
               sem_op.sem_num = 0;
               sem_op.sem_op = -1; // P operation
               sem_op.sem_flg = 0;
               if (semop(service_semaphore_ids[i], &sem_op, 1) == -1) {
                   printf("Service %d has completed its processing.\n", i);
               }
            }

            // Réception du résultat du service
            char result[256];
            read(pipe_orchestre_to_service[0], result, sizeof(result));
            // Envoi du résultat au client
            write(client_pipe, result, sizeof(result));
        }
        close(client_pipe);
    }

    // libération des ressources
    for (int i = 0; i < config_getNbServices(); i++) {
        char service_to_client_pipe[256];
        snprintf(service_to_client_pipe, sizeof(service_to_client_pipe), "service_to_client_%d", i);
        char client_to_service_pipe[256];
        snprintf(client_to_service_pipe, sizeof(client_to_service_pipe), "client_to_service_%d", i);
        unlink(service_to_client_pipe);
        unlink(client_to_service_pipe);
    }
    unlink(PIPE_O2C);
    unlink(PIPE_C2O);
    semctl(semaphore_id, 0, IPC_RMID);
    
    return EXIT_SUCCESS;
}
