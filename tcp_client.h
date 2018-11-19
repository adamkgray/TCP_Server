/*
 * TCP client
 * Description: A wrapper for TCP transactions
 * Purpose: Teach myself about networking
 */

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>     /* strlen */
#include <stdio.h>      /* printf, fprintf */
#include <sys/socket.h> /* socket, bind, listen, accept */
#include <unistd.h>     /* read, write */
#include <netinet/in.h> /* sockaddr_in, socklen_t */
#include <netdb.h>      /* gethostbyname */

#define SOCKET_ERR      -1
#define HOSTNAME_ERR    -2
#define CONNECTION_ERR  -3
#define WRITE_ERR       -4
#define READ_ERR        -5

typedef struct hostent hostent_t;
typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

int8_t tcp_client(char host[], uint32_t port, char message[], char buffer[], uint32_t buffer_size) {
    uint32_t socket_fd;
    uint32_t bytes_processed;
    hostent_t * server;
    sockaddr_in_t server_address;

    /* Open socket, get file descriptor */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return SOCKET_ERR;
    }

    /* Get host */
    server = gethostbyname(host);
    if (server == NULL) {
        return HOSTNAME_ERR;
    }

    /* Create server address */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);

    /* Connect */
    if (connect(socket_fd, (sockaddr_t *)&server_address, sizeof(server_address)) < 0) {
        return CONNECTION_ERR;
    }

    /* Write to server */
    bytes_processed = write(socket_fd, message, strlen(message));
    if (bytes_processed < 0) {
        return WRITE_ERR;
    }

    /* Read server response */
    bytes_processed = read(socket_fd, buffer, buffer_size);
    if (bytes_processed < 0) {
        return READ_ERR;
    }

    /* Close socket */
    close(socket_fd);

    return 0;
}

#endif
