#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../UTILS/myassert.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"
#include "service_sigma.h"

// Définition éventuelle de types pour stocker les données
typedef struct {
    float *data; // Tableau de float pour stocker les données
    int size;    // Taille du tableau
} SigmaData;

/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, SigmaData *sigmaData) {
    // Lire la taille des données
    ssize_t bytes_read = read(fd_pipe_from_client, &sigmaData->size, sizeof(sigmaData->size));
    myassert(bytes_read == sizeof(sigmaData->size), "Erreur lors de la lecture de la taille des données");

    // Allouer de la mémoire pour les données
    sigmaData->data = malloc(sigmaData->size * sizeof(float));
    myassert(sigmaData->data != NULL, "Erreur d'allocation de mémoire pour les données");

    // Lire les données du tube
    bytes_read = read(fd_pipe_from_client, sigmaData->data, sigmaData->size * sizeof(float));
    myassert((size_t)bytes_read == sigmaData->size * sizeof(float), "Erreur lors de la lecture des données");
}

// fonction de traitement des données
static float computeResult(SigmaData sigmaData) {
    float sum = 0.0f;
    for (int i = 0; i < sigmaData.size; i++) {
        sum += sigmaData.data[i]; 
    }
    return sum; 
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, float result) {
    // Écrire le résultat dans le tube
    ssize_t bytes_written = write(fd_pipe_to_client, &result, sizeof(result));
    myassert(bytes_written == sizeof(result), "Erreur lors de l'écriture du résultat");
}

/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_sigma(int fd_pipe_from_client, int fd_pipe_to_client) {
    // initialisations diverses
    SigmaData sigmaData;

    // Recevoir les données du client
    receiveData(fd_pipe_from_client, &sigmaData);

    // Traiter les données
    float result = computeResult(sigmaData);

    // Envoyer le résultat au client
    sendResult(fd_pipe_to_client, result);

    // Libération de la mémoire
    free(sigmaData.data);

    // libération éventuelle de ressources
}
