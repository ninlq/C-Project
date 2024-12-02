#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../CLIENT_SERVICE/client_service.h"
#include "client_sigma.h"


/*----------------------------------------------*
 * usage pour le client sigma
 *----------------------------------------------*/

static void usage(const char *exeName, const char *numService, const char *message)
{
    fprintf(stderr, "Client sigma de float\n");
    fprintf(stderr, "usage : %s %s <nbThreads> <f1> <f2> .. <fn>\n", exeName, numService);
    fprintf(stderr, "        %s           : numéro du service\n", numService);
    fprintf(stderr, "        <nbThreads>   : nombre de threads\n");
    fprintf(stderr, "        <f1> ... <fn> : les nombres à tester (au moins un)\n");
    fprintf(stderr, "exemple d'appel :\n");
    fprintf(stderr, "    %s %s 2 6.3 5.8 -2.33 0.0 8.34 12.98\n", exeName, numService);
    fprintf(stderr, "    -> 6 nombres à tester avec 2 threads\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------*
 * fonction de vérification des paramètres
 *----------------------------------------------*/

void client_sigma_verifArgs(int argc, char * argv[])
{
    if (argc < 4)
        usage(argv[0], argv[1], "nombre d'arguments");
    // éventuellement d'autres tests
}


/*----------------------------------------------*
 * fonctions de communication avec le service
 *----------------------------------------------*/

// ---------------------------------------------
// fonction d'envoi des données du client au service
// Les paramètres sont
// - le file descriptor du tube de communication vers le service
// - le nombre de threads que doit utiliser le service
// - le tableau de float dont on veut la somme
static void sendData(int fd_pipe_to_service, int nbThreads, float *data, int count)
{
    write(fd_pipe_to_service, &nbThreads, sizeof(nbThreads));
    write(fd_pipe_to_service, data, sizeof(float) * count);
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - autre chose si nécessaire
static void receiveResult(int fd_pipe_from_service)
{
    float result;
    read(fd_pipe_from_service, &result, sizeof(result));
    printf("Le résultat de sigma est : %f\n", result);
}


// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : nombre de threads
//    - argv[3] à argv[argc-1]: les nombres flottants
void client_sigma(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char * argv[])
{
    client_sigma_verifArgs(argc, argv);
    int nbThreads = atoi(argv[2]);
    int count = argc - 3;
    float *data = malloc(sizeof(float) * count);

    for (int i = 0; i < count; i++) {
        data[i] = strtof(argv[i + 3], NULL);
    }

    sendData(fd_pipe_to_service, nbThreads, data, count);
    receiveResult(fd_pipe_from_service);
    free(data);
}

