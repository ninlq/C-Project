#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

#include <stdbool.h>

void client_service_verifArgs(int argc, char *argv[]);
void client_somme(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]);
void client_sigma(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]);
void client_compression(int fd_pipe_to_service, int fd_pipe_from_service, int argc, char *argv[]);

#endif // CLIENT_SERVICE_H