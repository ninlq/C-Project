#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "../UTILS/myassert.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"
#include "service_compression.h"

// Définition d'une structure pour stocker les données à compresser
typedef struct {
    char *data; // Chaîne à compresser
    size_t length; // Longueur de la chaîne
} CompressionData;

/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, CompressionData *compressionData) {
    // Lire la longueur de la chaîne
    ssize_t bytes_read = read(fd_pipe_from_client, &compressionData->length, sizeof(compressionData->length));
    myassert(bytes_read == sizeof(compressionData->length), "Erreur lors de la lecture de la longueur des données");

    // Allouer de la mémoire pour la chaîne
    compressionData->data = malloc(compressionData->length * sizeof(char));
    myassert(compressionData->data != NULL, "Erreur d'allocation de mémoire pour les données");

    // Lire la chaîne du tube
    bytes_read = read(fd_pipe_from_client, compressionData->data, compressionData->length);
    myassert(bytes_read == compressionData->length, "Erreur lors de la lecture des données");
}

// fonction de traitement des données
static void computeResult(CompressionData compressionData, char *result) {
    // Compression simple : supprimer les caractères dupliqués
    size_t index = 0;
    for (size_t i = 0; i < compressionData.length; i++) {
        if (i == 0 || compressionData.data[i] != compressionData.data[i - 1]) {
            result[index++] = compressionData.data[i];
        }
    }
    result[index] = '\0'; // Terminer la chaîne résultante
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, const char *result) {
    // Écrire le résultat dans le tube
    ssize_t bytes_written = write(fd_pipe_to_client, result, strlen(result) + 1); // +1 pour le caractère nul
    myassert(bytes_written == strlen(result) + 1, "Erreur lors de l'écriture du résultat");
}

/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_compression(int fd_pipe_from_client, int fd_pipe_to_client) {
    // initialisations diverses
    CompressionData compressionData;

    // Recevoir les données du client
    receiveData(fd_pipe_from_client, &compressionData);

    // Préparer un buffer pour le résultat
    char *result = malloc(compressionData.length * sizeof(char)); // Allouer un espace pour le résultat
    myassert(result != NULL, "Erreur d'allocation de mémoire pour le résultat");

    // Traiter les données
    computeResult(compressionData, result);

    // Envoyer le résultat au client
    sendResult(fd_pipe_to_client, result);

    // Libération de la mémoire
    free(compressionData.data);
    free(result);
}