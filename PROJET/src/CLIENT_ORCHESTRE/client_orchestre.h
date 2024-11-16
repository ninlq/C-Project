#ifndef CLIENT_ORCHESTRE_H
#define CLIENT_ORCHESTRE_H

#include <stdbool.h>

// Définition des noms des tubes (pipes nommés)
#define PIPE_O2C "pipe_o2c" // Tube de l'orchestre vers le client
#define PIPE_C2O "pipe_c2o" // Tube du client vers l'orchestre

// Fonction pour initialiser la communication avec l'orchestre
bool init_communication_with_orchestre();

// Fonction pour envoyer une requête à l'orchestre
bool send_request_to_orchestre(int service_number, const char *params);

// Fonction pour recevoir une réponse de l'orchestre
bool receive_response_from_orchestre(char *response_buffer, size_t buffer_size);

// Fonction pour fermer la communication avec l'orchestre
void close_communication_with_orchestre();

#endif // CLIENT_ORCHESTRE_H