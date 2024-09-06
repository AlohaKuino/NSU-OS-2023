#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    pid_t pid;
    int status;
    if (argc < 2){
        fprintf(stderr, "No command passed\n");
        exit(EXIT_FAILURE);
    }
    switch(pid = fork()) {
        case -1:
                perror("Fork error");
                exit(EXIT_FAILURE);
                break;
        case 0:
                execvp(argv[1], argv+1);
                perror("Exec error");
                exit(EXIT_FAILURE);
                break;
        default: 
            if (waitpid(pid, &status, 0) == -1) {
                perror("Waitpid error");
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status)) {
                printf("\nExit code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("\nSignal terminated procces: %d\n", WTERMSIG(status));
            } else {
                fprintf(stderr, "\nProcess was terminated in a bad way\n");
            }
    }
    return 0;
}
