#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../UTILS/myassert.h"
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
    int fd_pipe_from_client = open(nom_tube_client_vers_service, O_RDONLY);
    int fd_pipe_to_client = open(nom_tube_service_vers_client, O_WRONLY);
    myassert(fd_pipe_from_client != -1, "Erreur lors de l'ouverture du tube du client vers le service");
    myassert(fd_pipe_to_client != -1, "Erreur lors de l'ouverture du tube du service vers le client");

    while (true)
    {
        // attente d'un code de l'orchestre (via tube anonyme)
        int code_orchestre;
        read(fd_tube_anonyme, &code_orchestre, sizeof(code_orchestre));

        // si code de fin
        //    sortie de la boucle
        if (code_orchestre == SERVICE_ARRET) {
            break;
        }

        // sinon
        //    réception du mot de passe de l'orchestre
        char password[256];
        receive_data_from_service(fd_tube_anonyme, password, sizeof(password));

        //    ouverture des deux tubes nommés avec le client (déjà)

        //    attente du mot de passe du client
        char client_password[256];
        read(fd_pipe_from_client, client_password, sizeof(client_password));

        //    si mot de passe incorrect
        //        envoi au client d'un code d'erreur
        if (strcmp(password, client_password) != 0) {
            int error_code = -1; 
            write(fd_pipe_to_client, &error_code, sizeof(error_code));
            continue;
        }

        //    sinon
        //        envoi au client d'un code d'acceptation
        int accept_code = 1; 
        write(fd_pipe_to_client, &accept_code, sizeof(accept_code));

        //        appel de la fonction de communication avec le client :
        //            une fct par service selon numService (cf. argv[1]) :
        //                   . service_somme
        //                ou . service_compression
        //                ou . service_sigma
        switch (num_service) {
            case SERVICE_SOMME:
                service_somme(fd_pipe_from_client, fd_pipe_to_client);
                break;
            case SERVICE_COMPRESSION:
                service_compression(fd_pipe_from_client, fd_pipe_to_client);
                break;
            case SERVICE_SIGMA:
                service_sigma(fd_pipe_from_client, fd_pipe_to_client);
                break;
            default:
                fprintf(stderr, "Service inconnu\n");
                exit(EXIT_FAILURE);
        }

        //        attente de l'accusé de réception du client
        int ack;
        read(fd_pipe_from_client, &ack, sizeof(ack));

        //    finsi
        //    fermeture ici des deux tubes nommés avec le client
        close(fd_pipe_from_client);
        close(fd_pipe_to_client);

        //    modification du sémaphore pour prévenir l'orchestre de la fin
        notify_orchestre_of_completion(semaphore_key);

        // finsi
    }

    // libération éventuelle de ressources
    close(fd_tube_anonyme);
    
    return EXIT_SUCCESS;
}
