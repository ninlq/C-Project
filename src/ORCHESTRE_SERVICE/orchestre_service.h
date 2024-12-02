#ifndef ORCHESTRE_SERVICE_H
#define ORCHESTRE_SERVICE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>

#define MAX_SERVICES 10

key_t get_semaphore_key(int num_service);
int create_semaphore(int num_service);
void delete_semaphore(int sem_id);
void semaphore_wait(int sem_id);
void semaphore_signal(int sem_id);

#endif // ORCHESTRE_SERVICE_H