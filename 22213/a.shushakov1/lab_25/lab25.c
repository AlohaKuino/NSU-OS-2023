#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int main() {
    int fildes[2];
    if (pipe(fildes) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            close(fildes[0]);
            close(fildes[1]);
            exit(EXIT_FAILURE);
        case 0: 
            close(fildes[1]);
            int in = fildes[0];
            char ch;
            while (read(in, &ch, 1)) {
                printf("%c", tolower(ch));
            }
            printf("\n");
            close(in);
            break;
       default:  
            close(fildes[0]);
            char buffer[BUFSIZ];
            ssize_t bytesRead;

            while ((bytesRead = read(STDIN_FILENO, buffer, BUFSIZ)) > 0) {
                if (write(fildes[1], buffer, bytesRead) == -1) {
                    perror("write");
                    close(fildes[1]);
                    exit(EXIT_FAILURE);
                }
            }

            close(fildes[1]);  

            if (waitpid(pid, NULL, 0) == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }
    }
    exit(EXIT_SUCCESS);
}
