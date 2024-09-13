#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/select.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/socket"

void deleteSocketFile(int signo) {
    unlink(SOCKET_PATH);
    printf("\nServer socket file removed\n");
    exit(EXIT_SUCCESS);
}

int main() {
    int server_socket, client_socket, max_socket_count, select_ready;
    struct sockaddr_un server_address;
    fd_set active_sockets, ready_sockets;
    char buffer[BUFSIZ];

    signal(SIGINT, deleteSocketFile);
    signal(SIGTERM, deleteSocketFile);

    if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_un)) == -1) {
        perror("bind error");
        unlink(SOCKET_PATH);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 2) == -1) {
        perror("listen error");
        unlink(SOCKET_PATH);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_sockets);
    FD_SET(server_socket, &active_sockets);
    max_socket_count = server_socket;

    printf("Server is listening on %s\n", SOCKET_PATH);

    while (1) {
        ready_sockets = active_sockets;

        select_ready = select(max_socket_count + 1, &ready_sockets, NULL, NULL, NULL);
        if (select_ready == -1) {
            perror("select error");
            unlink(SOCKET_PATH);
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_socket_count; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == server_socket) {
                    if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
                        perror("accept error");
                        unlink(SOCKET_PATH);
                    } else {
                        FD_SET(client_socket, &active_sockets);
                        if (client_socket > max_socket_count) {
                            max_socket_count = client_socket;
                        }
                        printf("New client connected\n");
                    }
                } else {
                    int num_bytes = read(i, buffer, sizeof(buffer) - 1);
                    if (num_bytes > 0) {
                        buffer[num_bytes] = '\0';
                        for (int j = 0; buffer[j]; j++) {
                            buffer[j] = toupper((unsigned char) buffer[j]);
                        }
                        printf("Message: %s", buffer);
                    } else {
                        if (num_bytes == 0) {
                            printf("Client disconnected\n");
                        } else {
                            perror("read error");
                        }
                        close(i);
                        FD_CLR(i, &active_sockets);
                    }
                }
            }
        }
    }

    close(server_socket);
    unlink(SOCKET_PATH);
    return 0;
}
