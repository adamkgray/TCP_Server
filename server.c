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

int main(int argc, char ** argv) {
    /* Check for correct number of arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    /* Get port number */
    uint32_t PORT = atoi(argv[1]);
    if (!PORT) {
        fprintf(stderr, "ERROR invalid port\n");
        return 1;
    }

    /* Open socket, create parent file descriptor */
    uint32_t parent_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (parent_fd < 0) {
        fprintf(stderr, "ERROR opening socket\n");
        return 1;
    }

    /* Bind socket to port */
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    if (bind(parent_fd, (struct sockaddr *)(&server_address), sizeof(server_address))) {
        fprintf(stderr, "ERROR binding\n");
        return 1;
    }

    /* Start listening (allow 16 queued connections) */
    if (listen(parent_fd, 0x10) < 0) {
        fprintf(stderr, "ERROR listening\n");
        return 1;
    }
    printf("TCP server is now listening on port %d ...\n", PORT);

    /* Client vars */
    struct sockaddr_in client_address;
    uint32_t client_length = sizeof(client_address);

    /* Ye olde infinite loop */
    for (;;) {

        /* Wait for connection request */
        uint32_t child_fd = accept(parent_fd, (struct sockaddr *)&server_address, (socklen_t *)&client_length);
        if (child_fd < 0) {
            fprintf(stderr, "ERROR accepting\n");
            return 1;
        }

        uint32_t BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        uint32_t bytes_proccessed = 0;
        memset(buffer, 0, sizeof(buffer));

        /* Listen */
        bytes_proccessed = read(child_fd, buffer, BUFFER_SIZE);
        if (bytes_proccessed < 0) {
            fprintf(stderr, "ERROR reading\n");
            return 1;
        }

        /* Log incoming request */
        printf("\nReceived %d bytes\n", bytes_proccessed);
        printf("Message:\n%s\n", buffer);

        /* Respond */
        char response[] = "TCP server response!\n";
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
