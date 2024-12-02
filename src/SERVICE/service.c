#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>

#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"
#include "service.h"
#include "service_somme.h"
#include "service_compression.h"
#include "service_sigma.h"




static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <num_service> <clé_sémaphore> <fd_tube_anonyme> "
            "<nom_tube_service_vers_client> <nom_tube_client_vers_service>\n",
            exeName);
    fprintf(stderr, "        <num_service>     : entre 0 et %d\n", SERVICE_NB - 1);
    fprintf(stderr, "        <clé_sémaphore>   : entre ce service et l'orchestre (clé au sens ftok)\n");
    fprintf(stderr, "        <fd_tube_anonyme> : entre ce service et l'orchestre\n");
    fprintf(stderr, "        <nom_tube_...>    : noms des deux tubes nommés reliés à ce service\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}




/*----------------------------------------------*
 * fonction main
 *----------------------------------------------*/
int main(int argc, char * argv[])
{
    if (argc != 6)
        usage(argv[0], "nombre paramètres incorrect");


    // initialisations diverses : analyse de argv
    int num_service = atoi(argv[1]);
    //key_t semaphore_key = atoi(argv[2]);
    int fd_tube_anonyme = atoi(argv[3]);
    const char *nom_tube_service_vers_client = argv[4];
    const char *nom_tube_client_vers_service = argv[5];


    int fd_pipe_to_client = open(nom_tube_service_vers_client, O_WRONLY);
    int fd_pipe_from_client = open(nom_tube_client_vers_service, O_RDONLY);


    while (true)
    {
        // attente d'un code de l'orchestre (via tube anonyme)
        int code;
        read(fd_tube_anonyme, &code, sizeof(int));
        // si code de fin
        //    sortie de la boucle
        if (code == -1)
            break;
        // sinon
        //    réception du mot de passe de l'orchestre
        char password[256];
        read(fd_tube_anonyme, password, sizeof(password));
        //    ouverture des deux tubes nommés avec le client
        //    attente du mot de passe du client
        char client_password[256];
        read(fd_pipe_from_client, client_password, sizeof(client_password));
        //    si mot de passe incorrect
        //        envoi au client d'un code d'erreur
        if (strcmp(password, client_password) != 0) {
            write(fd_pipe_to_client, "Erreur: Mot de passe incorrect", 30);
            continue;
        }
        //    sinon
        //        envoi au client d'un code d'acceptation
        write(fd_pipe_to_client, "Accepté", 8);
        //        appel de la fonction de communication avec le client :
        //            une fct par service selon numService (cf. argv[1]) :
        //                   . service_somme
        //                ou . service_compression
        //                ou . service_sigma
        switch (num_service) {
            case 0:
                service_somme(fd_pipe_from_client, fd_pipe_to_client);
                break;
            case 1:
                service_compression(fd_pipe_from_client, fd_pipe_to_client);
                break;
            case 2:
                service_sigma(fd_pipe_from_client, fd_pipe_to_client);
                break;
            default:
                fprintf(stderr, "Service invalide\n");
                break;
        }
        //        attente de l'accusé de réception du client
        char ack[256];
        read(fd_pipe_from_client, ack, sizeof(ack));
        //    finsi
        //    fermeture ici des deux tubes nommés avec le client
        close(fd_pipe_to_client);
        close(fd_pipe_from_client);
        //    modification du sémaphore pour prévenir l'orchestre de la fin
        // finsi
    }


    // libération éventuelle de ressources
    close(fd_tube_anonyme);
   
    return EXIT_SUCCESS;
}



