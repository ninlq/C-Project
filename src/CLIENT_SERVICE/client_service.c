#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "client_service.h"
#include "../UTILS/myassert.h"

static int fd_pipe_to_service; 
static int fd_pipe_from_service; 

void client_service_init(int num_service) {
    char pipe_to_service[256];
    char pipe_from_service[256];

    snprintf(pipe_to_service, sizeof(pipe_to_service), "pipe_c2s_%d", num_service);
    snprintf(pipe_from_service, sizeof(pipe_from_service), "pipe_s2c_%d", num_service);

    fd_pipe_to_service = open(pipe_to_service, O_WRONLY);
    myassert(fd_pipe_to_service != -1, "Erreur lors de l'ouverture du tube vers le service");

    fd_pipe_from_service = open(pipe_from_service, O_RDONLY);
    myassert(fd_pipe_from_service != -1, "Erreur lors de l'ouverture du tube depuis le service");
}

void client_service_send(const char *message) {
    ssize_t bytes_written = write(fd_pipe_to_service, message, strlen(message) + 1);
    myassert(bytes_written != -1, "Erreur lors de l'envoi du message au service");
}

void client_service_receive(char *buffer, size_t size) {
    ssize_t bytes_read = read(fd_pipe_from_service, buffer, size);
    myassert(bytes_read != -1, "Erreur lors de la réception du message depuis le service");
    myassert(bytes_read > 0, "Aucun message reçu du service");
}

void client_service_close() {
    close(fd_pipe_to_service);
    close(fd_pipe_from_service);
}

int client_service_get_fd_to_service() {
    return fd_pipe_to_service;
}

int client_service_get_fd_from_service() {
    return fd_pipe_from_service;
}



