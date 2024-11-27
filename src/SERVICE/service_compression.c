#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"

#include "service_compression.h"

// définition éventuelle de types pour stocker les données


/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, char *input, char *output)
{
    read(fd_pipe_from_client, input, 256);
    read(fd_pipe_from_client, output, 256);
}

// fonction de traitement des données
static void computeResult(const char *input, char *output)
{
    int len = strlen(input);
    int count = 1;
    int j = 0;

    for (int i = 1; i <= len; i++) {
        if (input[i] == input[i - 1]) {
            count++;
        } else {
            j += sprintf(&output[j], "%c%d", input[i - 1], count);
            count = 1;
        }
    }
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, const char *output)
{
    write(fd_pipe_to_client, output, strlen(output) + 1);
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_compression(int fd_pipe_from_client, int fd_pipe_to_client)
{
    char input[256];
    char output[256];

    receiveData(fd_pipe_from_client, input, output);
    computeResult(input, output);
    sendResult(fd_pipe_to_client, output);
}
