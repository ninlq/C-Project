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

#include "../UTILS/io.h"
#include "../UTILS/memory.h"
#include "../UTILS/myassert.h"

#include "../SERVICE/service.h"
#include "../CLIENT_ORCHESTRE/client_orchestre.h"
#include "../CLIENT_SERVICE/client_service.h"

#include "client_arret.h"
#include "client_somme.h"
#include "client_sigma.h"
#include "client_compression.h"

extern int pipe_o2c_fd;


static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <num_service> ...\n", exeName);
    fprintf(stderr, "        <num_service> : entre -1 et %d\n", SERVICE_NB - 1);
    fprintf(stderr, "                        -1 signifie l'arrêt de l'orchestre\n");
    fprintf(stderr, "        ...           : les paramètres propres au service\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char * argv[])
{
    if (argc < 2)
        usage(argv[0], "nombre paramètres incorrect");

    int numService = io_strToInt(argv[1]);
    if (numService < -1 || numService >= SERVICE_NB)
        usage(argv[0], "numéro service incorrect");

    // appeler la fonction de vérification des arguments
    //     une fct par service selon numService
    //            . client_arret_verifArgs
    //         ou . client_somme_verifArgs
    //         ou . client_compression_verifArgs
    //         ou . client_sigma_verifArgs
    switch (numService) {
        case -1:
            client_arret_verifArgs(argc, argv);
            break;
        case SERVICE_SOMME:
            client_somme_verifArgs(argc, argv);
            break;
        case SERVICE_COMPRESSION:
            client_compression_verifArgs(argc, argv);
            break;
        case SERVICE_SIGMA:
            client_sigma_verifArgs(argc, argv);
            break;
        default:
            usage(argv[0], "numéro service inconnu");
    }

    // initialisations diverses s'il y a lieu
    int semaphore_id;
    int service_semaphore_key = ftok("somefile", 'B'); 
    semaphore_id = semget(service_semaphore_key, 1, IPC_CREAT | 0666);

    // entrée en section critique pour communiquer avec l'orchestre
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;

    
    // ouverture des tubes avec l'orchestre
    int pipe_o2c_fd = open(PIPE_O2C, O_RDONLY);
    int pipe_c2o_fd = open(PIPE_C2O, O_WRONLY);
    if (pipe_o2c_fd < 0 || pipe_c2o_fd < 0) {
        fprintf(stderr, "Failed to open pipes with orchestre.\n");
        exit(EXIT_FAILURE);
    }

    // envoi à l'orchestre du numéro du service
    write(pipe_c2o_fd, &numService, sizeof(numService));

    // attente code de retour
    int returnCode;
    if (read(pipe_o2c_fd, &returnCode, sizeof(returnCode)) <= 0) {
        fprintf(stderr, "Failed to read return code from orchestre.\n");
        close_communication_with_orchestre();
        return EXIT_FAILURE;
    }
    // si code d'erreur
    //     afficher un message erreur
    if (returnCode < 0) {
        fprintf(stderr, "Error code received from orchestre: %d\n", returnCode);
        close_communication_with_orchestre();
        return EXIT_FAILURE;
    } 
    // sinon si demande d'arrêt (i.e. numService == -1)
    //     afficher un message    
    else if (numService == -1) {
        printf("Orchestre is stopping.\n");
    } else {
    // sinon
    //     récupération du mot de passe et des noms des 2 tubes
        char password[256];
        char tubeName1[256];
        char tubeName2[256];
        if (read(pipe_o2c_fd, password, sizeof(password)) <= 0 ||
            read(pipe_o2c_fd, tubeName1, sizeof(tubeName1)) <= 0 ||
            read(pipe_o2c_fd, tubeName2, sizeof(tubeName2)) <= 0) 
            {
            fprintf(stderr, "Failed to read password and tube names from orchestre.\n");
            close_communication_with_orchestre();
            return EXIT_FAILURE;
            }
    // finsi

    // envoi d'un accusé de réception à l'orchestre
    int ack = 1;
    write(pipe_c2o_fd, &ack, sizeof(ack));
    // fermeture des tubes avec l'orchestre
    close(pipe_o2c_fd);
    close(pipe_c2o_fd);
    // on prévient l'orchestre qu'on a fini la communication (cf. orchestre.c)
    // sortie de la section critique
    sem_op.sem_op = 1;
    semop(semaphore_id, &sem_op, 1);

    // si pas d'erreur et service normal
    //     ouverture des tubes avec le service
    int fd_pipe_to_service = open(tubeName2, O_WRONLY);
    int fd_pipe_from_service = open(tubeName1, O_RDONLY);
    if (fd_pipe_to_service < 0 || fd_pipe_from_service < 0) {
        fprintf(stderr, "Failed to open pipes to service.\n");
        close_communication_with_orchestre();
        return EXIT_FAILURE;
    }
    //     envoi du mot de passe au service
    write(fd_pipe_to_service, password, sizeof(password));
    //     attente de l'accusé de réception du service
    read(fd_pipe_from_service, &ack, sizeof(ack));
    //     si mot de passe non accepté
    //         message d'erreur
    if (ack != 1) {
        fprintf(stderr, "Mot de passe non accepté par le service.\n");
        close_communication_with_orchestre();
        return EXIT_FAILURE;
    } else {
    //     sinon
    //         appel de la fonction de communication avec le service :
    //             une fct par service selon numService :
    //                    . client_somme
    //                 ou . client_compression
    //                 ou . client_sigma
    switch (numService) {
        case SERVICE_SOMME:
            client_somme(fd_pipe_to_service, fd_pipe_from_service, argv);
            break;
        case SERVICE_COMPRESSION:
            client_compression(fd_pipe_to_service, fd_pipe_from_service, argv);
            break;
        case SERVICE_SIGMA:
            client_sigma(fd_pipe_to_service, fd_pipe_from_service, argc, argv);
            break;
        default:
            fprintf(stderr, "Numéro de service inconnu.\n");
            close_communication_with_orchestre();
            return EXIT_FAILURE;
    }
    //         envoi d'un accusé de réception au service
    ack = 1;
    write(fd_pipe_to_service, &ack, sizeof(ack));
    //     finsi
    //     fermeture des tubes avec le service
        close(fd_pipe_to_service);
        close(fd_pipe_from_service);
    }
    // finsi

    // libération éventuelle de ressources
    close_communication_with_orchestre();
    return EXIT_SUCCESS;
}
}
