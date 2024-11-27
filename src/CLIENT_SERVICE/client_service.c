#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../UTILS/myassert.h"

#include "client_service.h"

void client_service_verifArgs(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_service> <param1> <param2> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

void client_somme(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]) {
    myassert(argc >= 3, "Il manque des paramètres pour le service de somme");
    
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    
    write(fd_pipe_to_service, &a, sizeof(int));
    write(fd_pipe_to_service, &b, sizeof(int));
    
    int result;
    read(fd_pipe_from_service, &result, sizeof(int));
    
    printf("Résultat de la somme: %d\n", result);
}

void client_sigma(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]) {
    myassert(argc >= 3, "Il manque des paramètres pour le service sigma");
    
    int n = atoi(argv[1]);
    
    write(fd_pipe_to_service, &n, sizeof(int));
    
    int result;
    read(fd_pipe_from_service, &result, sizeof(int));
    
    printf("Résultat du sigma: %d\n", result);
}

void client_compression(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]) {
    myassert(argc >= 3, "Il manque des paramètres pour le service de compression");
    
    const char *input = argv[1];
    const char *output = argv[2];
    
    write(fd_pipe_to_service, input, strlen(input) + 1);
    write(fd_pipe_to_service, output, strlen(output) + 1);
    
    char response[256];
    read(fd_pipe_from_service, response, sizeof(response));
    
    printf("Résultat de la compression: %s\n", response);
}