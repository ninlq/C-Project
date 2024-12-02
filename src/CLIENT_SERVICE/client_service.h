#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

#include <stdbool.h>
#include <fcntl.h>

void client_service_init(int num_service);

void client_service_send(const char *message);

void client_service_receive(char *buffer, size_t size);

void client_service_close();

int client_service_get_fd_to_service();

int client_service_get_fd_from_service();

#endif // CLIENT_SERVICE_H
