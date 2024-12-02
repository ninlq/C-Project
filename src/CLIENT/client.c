#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>

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
        case 0:
            client_somme_verifArgs(argc, argv);
            break;
        case 1:
            client_compression_verifArgs(argc, argv);
            break;
        case 2:
            client_sigma_verifArgs(argc, argv);
            break;
        default:
            usage(argv[0], "numéro service incorrect");
            break;
    }

    // initialisations diverses s'il y a lieu

    // entrée en section critique pour communiquer avec l'orchestre
    
    // ouverture des tubes avec l'orchestre
     client_orchestre_init();

    // envoi à l'orchestre du numéro du service
    client_orchestre_send(argv[1]);

    // attente code de retour
    char response[256];
    client_orchestre_receive(response, sizeof(response));
    // si code d'erreur
    //     afficher un message erreur
    if (strcmp(response, "Erreur") == 0) {
        fprintf(stderr, "Erreur lors de la communication avec l'orchestre.\n");
        client_orchestre_close();
        return EXIT_FAILURE;
    // sinon si demande d'arrêt (i.e. numService == -1)
    //     afficher un message
    } else if (numService == -1) {
        printf("Orchestre arrêté avec succès.\n");
        client_orchestre_close();
        return EXIT_SUCCESS; 
    // sinon
    //     récupération du mot de passe et des noms des 2 tubes
    // finsi
    } else {
        char password[256];
        char tube_service_to_client[256];
        char tube_client_to_service[256];
        sscanf(response, "%s %s %s", password, tube_service_to_client, tube_client_to_service);
    // envoi d'un accusé de réception à l'orchestre
    client_orchestre_send("Accusé de réception");
    // fermeture des tubes avec l'orchestre
    client_orchestre_close();
    // on prévient l'orchestre qu'on a fini la communication (cf. orchestre.c)
    // sortie de la section critique
    // si pas d'erreur et service normal
    //     ouverture des tubes avec le service
    client_service_init(numService);
    //     envoi du mot de passe au service
    client_service_send(password);
    //     attente de l'accusé de réception du service
    }
    char ack[256];
    client_service_receive(ack, sizeof(ack));
    //     si mot de passe non accepté
    //         message d'erreur
    if (strcmp(ack, "Accepté") != 0) {
        fprintf(stderr, "Mot de passe incorrect.\n");
        client_service_close();
        client_orchestre_close();
        return EXIT_FAILURE;
    //     sinon
    //         appel de la fonction de communication avec le service :
    //             une fct par service selon numService :
    //                    . client_somme
    //                 ou . client_compression
    //                 ou . client_sigma
    //         envoi d'un accusé de réception au service
    //     finsi
    } else {
        switch (numService) {
            case 0:
                client_somme(client_service_get_fd_to_service(), client_service_get_fd_from_service(), argc, argv);
                break;
            case 1:
                client_compression(client_service_get_fd_to_service(), client_service_get_fd_from_service(), argc, argv);
                break;
            case 2:
                client_sigma(client_service_get_fd_to_service(), client_service_get_fd_from_service(), argc, argv);
                break;
            default:
                fprintf(stderr, "Service invalide\n");
                break;
        }
        client_service_send("Fin de communication");
    }
    //     fermeture des tubes avec le service
    client_service_close();
    client_orchestre_close();
    // finsi




    // libération éventuelle de ressources
    
    return EXIT_SUCCESS;
}
