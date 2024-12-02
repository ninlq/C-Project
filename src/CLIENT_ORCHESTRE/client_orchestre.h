#ifndef CLIENT_ORCHESTRE_H
#define CLIENT_ORCHESTRE_H

#include <stdbool.h>
#include <fcntl.h>

void client_orchestre_init();
void client_orchestre_send(const char *message);
void client_orchestre_receive(char *buffer, size_t size);
void client_orchestre_wait_semaphore();
void client_orchestre_signal_semaphore();
void client_orchestre_close();

#endif // CLIENT_ORCHESTRE_H