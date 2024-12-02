#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "../UTILS/myassert.h"

#include "client_orchestre.h"

// Descripteurs de tubes
static int fd_service_to_client; 
static int fd_client_to_service; 
static int sem_id;

// Fonction pour initialiser la communication avec l'orchestre
void client_orchestre_init() {
    // Ouverture des tubes nommés
    fd_service_to_client = open("src/pipe_o2c", O_RDONLY);
    myassert(fd_service_to_client != -1, "Erreur lors de l'ouverture du tube service vers client");

    fd_client_to_service = open("src/pipe_c2o", O_WRONLY);
    myassert(fd_client_to_service != -1, "Erreur lors de l'ouverture du tube client vers service");

    // Initialisation du sémaphore
    key_t key = ftok("CLIENT_ORCHESTRE/client_orchestre.h", 5);
    myassert(key != -1, "Erreur lors de la génération de la clé pour le sémaphore");

    sem_id = semget(key, 1, IPC_CREAT | 0666);
    myassert(sem_id != -1, "Erreur lors de la création du sémaphore");
}

// Fonction pour envoyer un message à l'orchestre
void client_orchestre_send(const char *message) {
    ssize_t bytes_written = write(fd_client_to_service, message, strlen(message) + 1);
    myassert(bytes_written != -1, "Erreur lors de l'envoi du message à l'orchestre");
}

// Fonction pour recevoir un message de l'orchestre
void client_orchestre_receive(char *buffer, size_t size) {
    ssize_t bytes_read = read(fd_service_to_client, buffer, size);
    myassert(bytes_read != -1, "Erreur lors de la réception du message de l'orchestre");
    myassert(bytes_read > 0, "Aucun message reçu de l'orchestre");
}

// Fonction pour gérer le sémaphore
void client_orchestre_wait_semaphore() {
    struct sembuf p = {0, -1, 0}; // P operation (wait)
    myassert(semop(sem_id, &p, 1) != -1, "Erreur lors de l'attente du sémaphore");
}

void client_orchestre_signal_semaphore() {
    struct sembuf v = {0, 1, 0}; // V operation (signal)
    myassert(semop(sem_id, &v, 1) != -1, "Erreur lors de la signalisation du sémaphore");
}

// Fonction pour fermer les tubes
void client_orchestre_close() {
    close(fd_service_to_client);
    close(fd_client_to_service);
    // Optionnel : supprimer le sémaphore si nécessaire
    semctl(sem_id, 0, IPC_RMID);
}
