#ifndef ORCHESTRE_SERVICE_H
#define ORCHESTRE_SERVICE_H

// Ici toutes les communications entre l'orchestre et les services :
// - le tube anonyme pour que l'orchestre envoie des données au service
// - le sémaphore pour que  le service indique à l'orchestre la fin
//   d'un traitement

// Déclaration des fonctions pour initialiser et gérer les communications
//void init_orchestre_service(int semaphore_key);
void send_data_to_service(int fd_pipe, const char *data);
void receive_data_from_service(int fd_pipe, char *buffer, size_t size);
void notify_orchestre_of_completion(int semaphore_id);

#endif
