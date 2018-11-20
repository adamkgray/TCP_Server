/*
 * TCP server
 * Description: The minimum code needed for a TCP server in C
 * Purpose: Teach myself about networking
 */

#include <stdint.h>
#include <stdlib.h>     /* atoi */
#include <string.h>     /* strlen */
#include <stdio.h>      /* printf, fprintf */
#include <sys/socket.h> /* socket, bind, listen, accept */
#include <unistd.h>     /* read, write */
#include <netinet/in.h> /* sockaddr_in, socklen_t */

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

#define bind_socket(parent_fd, p_server_address) \
    bind(parent_fd, (sockaddr_t *)p_server_address, sizeof(*p_server_address))

#define accept_connection(parent_fd, p_server_address, client_length) \
    accept(parent_fd, (sockaddr_t *)p_server_address, (socklen_t *)&client_length)

int main(int argc, char ** argv) {
    uint32_t parent_fd;                                 /* Parent file descriptor */
    uint32_t child_fd;                                  /* Child file descriptor */
    sockaddr_in_t client_address;                       /* Client connection */
    uint32_t client_length = sizeof(client_address);    /* Size of client */
    uint32_t port;                                      /* Port to listen on */
    uint32_t BUFFER_SIZE = 1024;                        /* Maximum number of chars to receive */
    char buffer[BUFFER_SIZE];                           /* Input buffer */
    uint32_t bytes_proccessed = 0;                      /* Bytes process by read & write system calls */

    /* Check for correct number of arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    /* Get port number */
    port = atoi(argv[1]);
    if (!port) {
        fprintf(stderr, "ERROR invalid port\n");
        return 1;
    }

    /* Open socket, create parent file descriptor */
    parent_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (parent_fd < 0) {
        fprintf(stderr, "ERROR opening socket\n");
        return 1;
    }

    /* Create server address */
    sockaddr_in_t server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    /* Bind socket */
    if (bind_socket(parent_fd, &server_address) < 0) {
        fprintf(stderr, "ERROR binding\n");
        return 1;
    }

    /* Start listening (allow 16 queued connections) */
    if (listen(parent_fd, 0x10) < 0) {
        fprintf(stderr, "ERROR listening\n");
        return 1;
    }
    printf("Thine TCP server doth listen on port %d ...\n", port);

    /* Ye olde infinite loop */
    for (;;) {

        /* Wait for connection request */
        child_fd = accept_connection(parent_fd, &server_address, client_length);
        if (child_fd < 0) {
            fprintf(stderr, "ERROR accepting\n");
            return 1;
        }

        /* Clear buffer */
        memset(buffer, 0, sizeof(buffer));

        /* Listen */
        bytes_proccessed = read(child_fd, buffer, BUFFER_SIZE);
        if (bytes_proccessed < 0) {
            fprintf(stderr, "ERROR reading\n");
            return 1;
        }

        /* Log incoming request */
        printf("\nReceived %d bytes\n", bytes_proccessed);
        printf("%s\n", buffer);

        /* Respond */
        char response[] = "Fare Thee Well!\n";
        bytes_proccessed = write(child_fd, response, strlen(response));
        if (bytes_proccessed < 0) {
            fprintf(stderr, "ERROR writing\n");
            return 1;
        }

        /* Close child */
        close(child_fd);
    }

    return 0;
}

