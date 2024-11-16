#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "orchestre_service.h"
#include "../UTILS/myassert.h"

// Fonction de test pour initialiser le service orchestral
void test_init_orchestre_service() {
    int semaphore_key = 1234; // Choisir une clé de sémaphore
    int service_num = 0; // Numéro du service

    // Appeler la fonction d'initialisation
    init_orchestre_service(service_num, semaphore_key);

    // Vérifier si le sémaphore a été créé avec succès
    int semaphore_id = semget(semaphore_key, 1, 0666);
    myassert(semaphore_id != -1, "Le sémaphore n'a pas été créé avec succès");
    
    // Libérer le sémaphore
    semctl(semaphore_id, 0, IPC_RMID);
}

// Fonction de test pour envoyer des données au service
void test_send_data_to_service() {
    int fd_pipe[2];
    if (pipe(fd_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    const char *data = "Données de test";
    send_data_to_service(fd_pipe[1], data);

    // Lire les données du tube pour vérifier
    char buffer[256];
    read(fd_pipe[0], buffer, sizeof(buffer));

    myassert(strcmp (buffer, data) == 0, "Les données envoyées ne correspondent pas aux données lues");

    close(fd_pipe[0]);
    close(fd_pipe[1]);
}

// Fonction principale pour exécuter les tests
int main() {
    test_init_orchestre_service();
    test_send_data_to_service();

    printf("Tous les tests ont réussi.\n");
    return 0;
}