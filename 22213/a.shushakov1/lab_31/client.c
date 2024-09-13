#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/socket"

int main() {
    int client_socket;
    struct sockaddr_un server_address;
    char buffer[BUFSIZ];

    if ((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_un)) == -1) {
        perror("connect error");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Enter text to send to server: ");
    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        if (write(client_socket, buffer, strlen(buffer)) == -1) {
            perror("write error");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        printf("Enter text to send to server: ");
    }

    close(client_socket);
    return 0;
}
