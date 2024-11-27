#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"

#include "service_sigma.h"

// définition éventuelle de types pour stocker les données


/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, int *nbThreads, float **data, int *count)
{
    read(fd_pipe_from_client, nbThreads, sizeof(int));
    *count = *nbThreads;
    *data = malloc(sizeof(float) * (*count));
    read(fd_pipe_from_client, *data, sizeof(float) * (*count));
}

// fonction de traitement des données
static void computeResult(int nbThreads, float *data, int count, float *result)
{
    float sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += data[i];
    }
    *result = sum / count;
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, float result)
{
    write(fd_pipe_to_client, &result, sizeof(result));
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_sigma(int fd_pipe_from_client, int fd_pipe_to_client)
{
    int nbThreads;
    float *data;
    int count;
    float result;

    receiveData(fd_pipe_from_client, &nbThreads, &data, &count);
    computeResult(nbThreads, data, count, &result);
    sendResult(fd_pipe_to_client, result);

    free(data);
}
