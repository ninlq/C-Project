#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>

#include "../CONFIG/config.h"
#include "../CLIENT_ORCHESTRE/client_orchestre.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../SERVICE/service.h"


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
    // - création d'un sémaphore pour que deux clients ne
    //   ne communiquent pas en même temps avec l'orchestre
    int fd_service_to_client[config_getNbServices()];
    int fd_client_to_service[config_getNbServices()];
        
    // lancement des services, avec pour chaque service :
    // - création d'un tube anonyme pour converser (orchestre vers service)
    // - un sémaphore pour que le service préviene l'orchestre de la
    //   fin d'un traitement
    // - création de deux tubes nommés (pour chaque service) pour les
    //   communications entre les clients et les services
    for (int i = 0; i < config_getNbServices(); i++) {
        char pipe_service_to_client[256];
        char pipe_client_to_service[256];

        snprintf(pipe_service_to_client, sizeof(pipe_service_to_client), "pipe_s2c_%d", i);
        snprintf(pipe_client_to_service, sizeof(pipe_client_to_service), "pipe_c2s_%d", i);

        mkfifo(pipe_service_to_client, 0666);
        mkfifo(pipe_client_to_service, 0666);

        fd_service_to_client[i] = open(pipe_service_to_client, O_WRONLY);
        fd_client_to_service[i] = open(pipe_client_to_service, O_RDONLY);
    }

    while (! fin)
    {
        // ouverture ici des tubes nommés avec un client
        // attente d'une demande de service du client
        char request[256];
        client_orchestre_receive(request, sizeof(request));
        
        // détecter la fin des traitements lancés précédemment via
        // les sémaphores dédiés (attention on n'attend pas la
        // fin des traitement, on note juste ceux qui sont finis)

        // analyse de la demande du client
        int numService = atoi(request);
        if (numService == -1) {
            fin = true;
            printf("Recevoir la demande d'arreter de client.\n");
            continue;
        }
        // si ordre de fin
        //     envoi au client d'un code d'acceptation (via le tube nommé)
        //     marquer le booléen de fin de la boucle
        // sinon si service non ouvert
        //     envoi au client d'un code d'erreur (via le tube nommé)
        if (!config_isServiceOpen(numService)) {
            write(fd_service_to_client[numService], "Erreur", 6);
            continue;
        }
        // sinon si service déjà en cours de traitement
        //     envoi au client d'un code d'erreur (via le tube nommé)
        // sinon
        //     envoi au client d'un code d'acceptation (via le tube nommé)
        write(fd_service_to_client[numService], "Accepté", 8);
        //     génération d'un mot de passe
        char password[256];
        snprintf(password, sizeof(password), "password_%d", numService);
        //     envoi d'un code de travail au service (via le tube anonyme)
        write(fd_client_to_service[numService], password, sizeof(password));
        //     envoi du mot de passe au service (via le tube anonyme)
        //     envoi du mot de passe au client (via le tube nommé)
        //     envoi des noms des tubes nommés au client (via le tube nommé)
        // finsi

        // attente d'un accusé de réception du client
        char response[256];
        read(fd_client_to_service[numService], response, sizeof(response));
        // fermer les tubes vers le client

        // il peut y avoir un problème si l'orchestre revient en haut de la
        // boucle avant que le client ait eu le temps de fermer les tubes
        // il faut attendre avec un sémaphore.
        // (en attendant on fait une attente d'1 seconde, à supprimer dès
        // que le sémaphore est en place)
        // attendre avec un sémaphore que le client ait fermé les tubes
        sleep(1);   // à supprimer
    }

    // attente de la fin des traitements en cours (via les sémaphores)

    // envoi à chaque service d'un code de fin

    // attente de la terminaison des processus services

    // libération des ressources
    for (int i = 0; i < config_getNbServices(); i++) {
            close(fd_service_to_client[i]);
            close(fd_client_to_service[i]);
    }

     config_exit();
    
    return EXIT_SUCCESS;
}
