#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "../CLIENT_SERVICE/client_service.h"
#include "client_compression.h"


/*----------------------------------------------*
 * usage pour le client compression
 *----------------------------------------------*/

static void usage(const char *exeName, const char *numService, const char *message)
{
    fprintf(stderr, "Client compression de chaîne\n");
    fprintf(stderr, "usage : %s %s <chaîne>\n", exeName, numService);
    fprintf(stderr, "        %s      : numéro du service\n", numService);
    fprintf(stderr, "        <chaine> : chaîne à compresser\n");
    fprintf(stderr, "exemple d'appel :\n");
    fprintf(stderr, "    %s %s \"aaabbcdddd\"\n", exeName, numService);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


/*----------------------------------------------*
 * fonction de vérification des paramètres
 *----------------------------------------------*/

void client_compression_verifArgs(int argc, char * argv[])
{
    if (argc != 3)
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
// - la chaîne devant être compressée
static void sendDataToService(int fd_pipe_to_service, const char *stringToCompress)
{
    size_t length = strlen(stringToCompress);
    write(fd_pipe_to_service, &length, sizeof(length));
    write(fd_pipe_to_service, stringToCompress, length);
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - autre chose si nécessaire
static void receiveResultFromService(int fd_pipe_from_service)
{
    // récupération de la chaîne compressée
    // affichage du résultat
    char compressedString[256];
    read(fd_pipe_from_service, compressedString, sizeof(compressedString));
    printf("Chaîne compressée : %s\n", compressedString);
}

// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : la chaîne à compresser
void client_compression(int fd_pipe_to_service, int fd_pipe_from_service, char *argv[])
{
    // variables locales éventuelles
    const char *stringToCompress = argv[2];
    sendDataToService(fd_pipe_to_service, stringToCompress);
    receiveResultFromService(fd_pipe_from_service);
}

