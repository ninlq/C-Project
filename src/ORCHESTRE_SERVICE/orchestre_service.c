#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#include "orchestre_service.h"
#include "../UTILS/myassert.h"

key_t get_semaphore_key(int num_service) {
    key_t key = ftok("ORCHESTRE_SERVICE/orchestre_service.h", 1 + num_service);
    myassert(key != -1, "Erreur lors de la génération de la clé du sémaphore");
    return key;
}

int create_semaphore(int num_service) {
    key_t key = get_semaphore_key(num_service);
    int sem_id = semget(key, 1, IPC_CREAT | 0666);
    myassert(sem_id != -1, "Erreur lors de la création du sémaphore");
    
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("Erreur lors de l'initialisation du sémaphore");
        exit(EXIT_FAILURE);
    }
    
    return sem_id;
}

void delete_semaphore(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Erreur lors de la suppression du sémaphore");
    }
}

void semaphore_wait(int sem_id) {
    struct sembuf p = {0, -1, 0}; 
    myassert(semop(sem_id, &p, 1) != -1, "Erreur lors de l'attente du sémaphore");
}

void semaphore_signal(int sem_id) {
    struct sembuf v = {0, 1, 0}; 
    myassert(semop(sem_id, &v, 1) != -1, "Erreur lors de la signalisation du sémaphore");
}
