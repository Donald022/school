#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define READ_BUFSIZE 4096

static long count_lines(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    long count = 0;
    char buf[READ_BUFSIZE];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                count++;
            }
        }
    }

    if (close(fd) < 0) {
        return 0;
    }
    return count;
}

static void process_path(const char *path, bool show_lines) {
    struct stat st;
    if (stat(path, &st) < 0) {
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        printf("%s\n", path);

        DIR *dir = opendir(path);
        if (dir == NULL) {
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            int concatlen = strlen(path) + 1 + strlen(entry->d_name) + 1;
            char *newpath = malloc(concatlen * sizeof(char));
            if (newpath == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            snprintf(newpath, concatlen, "%s/%s", path, entry->d_name);

            process_path(newpath, show_lines);

            free(newpath);
        }

        closedir(dir);
    } else if (S_ISREG(st.st_mode)) {
        if (show_lines) {
            printf("%s\t%ld\n", path, count_lines(path));
        } else {
            printf("%s\n", path);
        }
    } else {
        printf("%s\n", path);
    }
}

int main(int argc, char *argv[]) {
    const char *path = NULL;
    bool show_lines = false;

    if (argc == 2) {
        path = argv[1];
    } else if (argc == 3) {
        if (strcmp(argv[1], "-l") != 0) {
            exit(EXIT_FAILURE);
        }
        show_lines = true;
        path = argv[2];
    } else {
        exit(EXIT_FAILURE);
    }

    process_path(path, show_lines);
    return 0;
}
