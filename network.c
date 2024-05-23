#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void setup_client_socket(int *sock, const char *server_ip, int port)
{
    struct sockaddr_in serv_addr;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Socket creation error");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
    {
        error("Invalid address/ Address not supported");
    }

    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connection Failed");
    }
}


