#ifndef SERVICE_SIGMA_H
#define SERVICE_SIGMA_H

// on ne déclare ici que les fonctions appelables par le main

// fonction pour gérer la communication avec le client
void service_sigma(int fd_pipe_from_client, int fd_pipe_to_client);

#endif
