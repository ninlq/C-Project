#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "client_orchestre.h"
#include "../UTILS/myassert.h"

// Descripteurs de fichiers pour les tubes
static int pipe_o2c_fd; // Descripteur pour le tube de l'orchestre vers le client
static int pipe_c2o_fd; // Descripteur pour le tube du client vers l'orchestre

// Fonction pour initialiser la communication avec l'orchestre
bool init_communication_with_orchestre() {
    // Créer les tubes s'ils n'existent pas
    myassert(mkfifo(PIPE_O2C, 0666) != -1, "Erreur lors de la création du tube PIPE_O2C");
    myassert(mkfifo(PIPE_C2O, 0666) != -1, "Erreur lors de la création du tube PIPE_C2O");

    // Ouvrir les tubes
    pipe_o2c_fd = open(PIPE_O2C, O_RDONLY | O_NONBLOCK);
    pipe_c2o_fd = open(PIPE_C2O, O_WRONLY);
    
    myassert(pipe_o2c_fd != -1, "Erreur lors de l'ouverture du tube PIPE_O2C");
    myassert(pipe_c2o_fd != -1, "Erreur lors de l'ouverture du tube PIPE_C2O");
    
    return true;
}

// Fonction pour envoyer une requête à l'orchestre
bool send_request_to_orchestre(int service_number, const char *params) {
    char request[256];
    snprintf(request, sizeof(request), "%d %s", service_number, params);
    
    // Envoyer la requête à l'orchestre
    ssize_t bytes_written = write(pipe_c2o_fd, request, strlen(request));
    myassert(bytes_written != -1, "Erreur lors de l'envoi de la requête à l'orchestre");
    
    return true;
}

// Fonction pour recevoir une réponse de l'orchestre
bool receive_response_from_orchestre(char *response_buffer, size_t buffer_size) {
    // Recevoir la réponse de l'orchestre
    ssize_t bytes_read = read(pipe_o2c_fd, response_buffer, buffer_size - 1);
    if (bytes_read > 0) {
        response_buffer[bytes_read] = '\0'; // Terminer la chaîne par un caractère nul
        return true;
    }
    return false;
}

// Fonction pour fermer la communication avec l'orchestre
void close_communication_with_orchestre() {
    close(pipe_o2c_fd); // Fermer le descripteur du tube de l'orchestre
    close(pipe_c2o_fd); // Fermer le descripteur du tube du client
    unlink(PIPE_O2C); // Supprimer le tube nommé de l'orchestre
    unlink(PIPE_C2O); // Supprimer le tube nommé du client
}