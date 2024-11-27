#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"

#include "service_somme.h"

// définition éventuelle de types pour stocker les données


/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, float *n1, float *n2)
{
    read(fd_pipe_from_client, n1, sizeof(float));
    read(fd_pipe_from_client, n2, sizeof(float));
}

// fonction de traitement des données
static void computeResult(float n1, float n2, float *result)
{
    *result = n1 + n2;
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, float result)
{
    write(fd_pipe_to_client, &result, sizeof(result));
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_somme(int fd_pipe_from_client, int fd_pipe_to_client)
{
    float n1, n2, result;

    receiveData(fd_pipe_from_client, &n1, &n2);
    computeResult(n1, n2, &result);
    sendResult(fd_pipe_to_client, result);
}
