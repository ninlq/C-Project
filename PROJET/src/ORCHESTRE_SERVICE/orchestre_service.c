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
    // Écrire les données dans le tube anonyme
    write(fd_pipe, data, strlen(data) + 1);
}

// Fonction pour recevoir des données du service
void receive_data_from_service(int fd_pipe, char *buffer, size_t size) {
    // Lire les données du tube anonyme
    read(fd_pipe, buffer, size);
}

// Fonction pour notifier l'orchestre de la fin du traitement
void notify_orchestre_of_completion(int semaphore_id) {
    // Vider le sémaphore pour indiquer la fin du traitement
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1; // Incrémenter le sémaphore
    sem_op.sem_flg = 0;
    semop(semaphore_id, &sem_op, 1);
}
