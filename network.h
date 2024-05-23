#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "dOPRF.h"

#define BUFFER_SIZE 1024
#define SERVER_PORT_BASE 9080  // Base port for the first server
#define SERVER_COUNT CONST_N   // Number of servers

typedef struct {
    int server_index;
    int sock;
    RSS_i message;
    uint8_t response[ (sizeof(DRSS_i) * LAMBDA) + sizeof(DRSS_digest_i)];
} server_data_t;

void setup_client_socket(int *sock, const char *server_ip, int port);

#endif