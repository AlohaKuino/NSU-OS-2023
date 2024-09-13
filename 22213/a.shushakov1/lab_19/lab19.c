#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>

#define MAX_PATTERN_LEN 256

void sanitize_input(char *input) {
    if (strchr(input, '/')) {
        fprintf(stderr, "Error: '/' is not allowed in the pattern.\n");
        exit(EXIT_FAILURE);
    }
}

int main() {
    char pattern[MAX_PATTERN_LEN];

    printf("Enter the file name pattern: ");
    if (!fgets(pattern, sizeof(pattern), stdin)) {
        perror("Error reading input");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(pattern);
    if (pattern[len - 1] == '\n') {
        pattern[len - 1] = '\0';
    }

    sanitize_input(pattern);

    DIR *directory = opendir(".");

    if (!directory) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent64 *entry;
    int match_found = 0;

    while (1) {
        errno = 0;
        entry = readdir64(directory);

        if (entry == NULL) {
            if (errno != 0) {
                perror("Error reading directory");
                if (closedir(directory) == -1) {
                    perror("Error closing directory");
                }
                exit(EXIT_FAILURE);
            }
            break;
        }

        if (fnmatch(pattern, entry->d_name, 0) == 0) {
            printf("%s\n", entry->d_name);
            match_found = 1;
        }
    }

    if (!match_found) {
        fprintf(stderr, "No files matched the pattern: %s\n", pattern);
    }

    if (closedir(directory) == -1) {
        perror("Error closing directory");
        exit(EXIT_FAILURE);
    }

    return 0;
}
