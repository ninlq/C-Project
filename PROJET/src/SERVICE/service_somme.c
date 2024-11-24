#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../UTILS/myassert.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../CLIENT_SERVICE/client_service.h"
#include "service_somme.h"

// définition éventuelle de types pour stocker les données
typedef struct {
    int number1;
    int number2;
} SumData;


/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fd_pipe_from_client, SumData *data) {
    // Lire les deux nombres du tube
    ssize_t bytes_read1 = read(fd_pipe_from_client, &data->number1, sizeof(data->number1));
    myassert(bytes_read1 == sizeof(data->number1), "Erreur lors de la lecture du premier nombre");

    ssize_t bytes_read2 = read(fd_pipe_from_client, &data->number2, sizeof(data->number2));
    myassert(bytes_read2 == sizeof(data->number2), "Erreur lors de la lecture du deuxième nombre");
}

// fonction de traitement des données
static int computeResult(SumData data) {
    // Calculer la somme
    return data.number1 + data.number2;
}

// fonction d'envoi du résultat
static void sendResult(int fd_pipe_to_client, int result) {
    // Écrire le résultat dans le tube
    ssize_t bytes_written = write(fd_pipe_to_client, &result, sizeof(result));
    myassert(bytes_written == sizeof(result), "Erreur lors de l'écriture du résultat");
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_somme(int fd_pipe_from_client, int fd_pipe_to_client) {
    // initialisations diverses
    SumData data;

    // Recevoir les données du client
    receiveData(fd_pipe_from_client, &data);

    // Traiter les données
    int result = computeResult(data);

    // Envoyer le résultat au client
    sendResult(fd_pipe_to_client, result);

    // libération éventuelle de ressources
}
