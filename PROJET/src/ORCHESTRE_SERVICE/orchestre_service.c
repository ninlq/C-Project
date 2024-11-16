#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "../UTILS/myassert.h"

#include "orchestre_service.h"

// Fonction pour initialiser le service orchestral
void init_orchestre_service(int service_num, int semaphore_key) {
    // Créer un sémaphore pour synchroniser avec l'orchestre
    int semaphore_id = semget(semaphore_key, 1, IPC_CREAT | 0666);
    myassert_func(semaphore_id != -1, "Erreur lors de la création du sémaphore");
}

// Fonction pour envoyer des données au service
void send_data_to_service(int fd_pipe, const char *data) {
    myassert(fd_pipe >= 0, "Descripteur de tube invalide");
    myassert(data != NULL, "Données à envoyer ne doit pas être NULL");

    // Écrire les données dans le tube anonyme
    ssize_t bytes_written = write(fd_pipe, data, strlen(data) + 1);
    myassert(bytes_written != -1, "Erreur lors de l'écriture des données");
}

// Fonction pour recevoir des données du service
void receive_data_from_service(int fd_pipe, char *buffer, size_t size) {
    myassert(fd_pipe >= 0, "Descripteur de tube non valide");
    myassert(buffer != NULL, "Buffer ne doit pas être NULL");

    // Lire les données du tube anonyme
    ssize_t bytes_read = read(fd_pipe, buffer, size);
    myassert(bytes_read != -1, "Erreur lors de la lecture des données");
}

// Fonction pour notifier l'orchestre de la fin du traitement
void notify_orchestre_of_completion(int semaphore_id) {
    myassert(semaphore_id >= 0, "ID de sémaphore non valide");

    // Vider le sémaphore pour indiquer la fin du traitement
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1; // Incrémenter le sémaphore
    sem_op.sem_flg = 0;
    int result = semop(semaphore_id, &sem_op, 1);
    myassert(result != -1, "Échec de l'opération sur le sémaphore");
}