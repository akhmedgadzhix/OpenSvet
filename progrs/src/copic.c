#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>

void make_directories(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s", dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    for(p = tmp + 1; *p; p++) {
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}

void copy_file(const char *src, const char *dst) {
    int input_fd, output_fd;
    ssize_t ret_in, ret_out;
    char buffer[4096];
    mode_t mode = 0644;

    input_fd = open(src, O_RDONLY);
    if (input_fd == -1) {
        perror("open source file");
        exit(EXIT_FAILURE);
    }

    output_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (output_fd == -1) {
        perror("open destination file");
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    while ((ret_in = read(input_fd, buffer, sizeof(buffer))) > 0) {
        ret_out = write(output_fd, buffer, (ssize_t) ret_in);
        if (ret_out != ret_in) {
            perror("write");
            close(input_fd);
            close(output_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(input_fd);
    close(output_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *src = argv[1];
    char *dst = argv[2];
    char *dst_dir = strdup(dst);

    make_directories(dirname(dst_dir));
    free(dst_dir);

    copy_file(src, dst);

    printf("File copied from %s to %s\n", src, dst);

    return 0;
}

