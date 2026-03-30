// a program to build and run a c program. made with no love by Salbyo & Co. in vim.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_FILES 1024
#define CMD_SIZE 8192

int has_c_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".c") == 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s <file.c | directory>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    struct stat path_stat;

    if (stat(input, &path_stat) != 0) {
        perror("error");
        return 1;
    }

    char tmp_exec[] = "/tmp/crunXXXXXX";
    int fd = mkstemp(tmp_exec);
    if (fd == -1) {
        perror("mkstemp failed");
        return 1;
    }
    close(fd); // we just need the filename

    char command[CMD_SIZE] = {0};

    if (S_ISREG(path_stat.st_mode)) {
        if (!has_c_extension(input)) {
            printf("error: file must have a .c extension\n");
            unlink(tmp_exec);
            return 1;
        }

        snprintf(command, sizeof(command),
                 "gcc \"%s\" -o \"%s\"", input, tmp_exec);

    } else if (S_ISDIR(path_stat.st_mode)) {
        DIR *dir = opendir(input);
        if (!dir) {
            perror("opendir failed");
            unlink(tmp_exec);
            return 1;
        }

        struct dirent *entry;
        char files[CMD_SIZE] = {0};
        int found = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (has_c_extension(entry->d_name)) {
                found = 1;
                strncat(files, "\"", sizeof(files) - strlen(files) - 1);
                strncat(files, input, sizeof(files) - strlen(files) - 1);
                strncat(files, "/", sizeof(files) - strlen(files) - 1);
                strncat(files, entry->d_name, sizeof(files) - strlen(files) - 1);
                strncat(files, "\" ", sizeof(files) - strlen(files) - 1);
            }
        }
        closedir(dir);

        if (!found) {
            printf("error: no .c files found in the directory\n");
            unlink(tmp_exec);
            return 1;
        }

        snprintf(command, sizeof(command),
                 "gcc %s -o \"%s\"", files, tmp_exec);

    } else {
        printf("error: invalid input\n");
        unlink(tmp_exec);
        return 1;
    }

    // compile
    if (system(command) != 0) {
        unlink(tmp_exec);
        return 1;
    }

    // run compiled program
    if (system(tmp_exec) != 0) {
        unlink(tmp_exec);
        return 1;
    }

    // cleanup
    unlink(tmp_exec);

    return 0;
}