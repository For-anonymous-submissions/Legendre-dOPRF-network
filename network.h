#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "dOPRF.h"

#define SERVER_PORT_BASE 9080  // Base port for the first server

typedef struct {
    int server_index;
    int sock;
    RSS_i message;

#if (ADVERSARY == SEMIHONEST)
    uint8_t response[sizeof(f_elm_t) * LAMBDA];
#elif (ADVERSARY == MALICIOUS)
    uint8_t response[(sizeof(DRSS_i) * LAMBDA) + sizeof(DRSS_digest_i)];
#endif
} server_data_t;

void setup_client_socket(int *sock, const char *server_ip, int port);

#endif