#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

#include <stdbool.h>

// Ici toutes les communications entre les services et les clients :
// - les deux tubes nommés pour la communication bidirectionnelle

// Déclaration des fonctions de communication entre le client et le service
void sendData(int fd_pipe_to_service, int number1, int number2);
void receiveResult(int fd_pipe_from_service, const char *prefix);

#endif
