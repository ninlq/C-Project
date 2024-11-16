#include <stdio.h>
#include <unistd.h>

#include "../UTILS/myassert.h"

#include "client_service.h"

// Fonction pour envoyer des données au service
void sendData(int fd_pipe_to_service, int number1, int number2) {
    // Assertion pour vérifier que le descripteur de tube est valide
    myassert(fd_pipe_to_service >= 0, "Descripteur de tube invalide");
    
    // Écriture des deux nombres dans le tube vers le service
    ssize_t bytes_written1 = write(fd_pipe_to_service, &number1, sizeof(number1));
    myassert(bytes_written1 == sizeof(number1), "Erreur lors de l'écriture du premier nombre");

    ssize_t bytes_written2 = write(fd_pipe_to_service, &number2, sizeof(number2));
    myassert(bytes_written2 == sizeof(number2), "Erreur lors de l'écriture du deuxième nombre");
}

// Fonction pour recevoir le résultat du service
void receiveResult(int fd_pipe_from_service, const char *prefix) {
    int result;
    
    // Assertion pour vérifier que le descripteur de tube est valide
    myassert(fd_pipe_from_service >= 0, "Descripteur de tube invalide");

    // Lecture du résultat du service
    ssize_t bytes_read = read(fd_pipe_from_service, &result, sizeof(result));
    myassert(bytes_read == sizeof(result), "Erreur lors de la lecture du résultat");

    // Affichage du résultat avec le préfixe
    printf("%s%d\n", prefix, result);
}