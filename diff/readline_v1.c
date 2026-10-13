#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_LINE 512

int readline_v1(int fd, char *buf, int max) {
    int i = 0;
    char c;
    int n;

    while (i < max - 1) {
        n = read(fd, &c, 1);
        if (n < 0) return -1;
        
        if (n == 0) {
            return 0; 
        }

        if (c == '\n') {
            break;
        }
        buf[i++] = c;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: test_readline_v1 <filename>\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(2, "Cannot open %s\n", argv[1]);
        exit(1);
    }

    char line[MAX_LINE];
    int line_num = 1;
    int status;

    printf("=== DANG TEST READLINE (LAN THU 1 - VERSION LOI) ===\n");
    while ((status = readline_v1(fd, line, sizeof(line))) == 1) {
        printf("[Dong %d]: %s\n", line_num, line);
        line_num++;
    }

    if (status < 0) {
        printf("Loi doc file!\n");
    } else {
        printf("=== KET THUC DOC TEP (EOF) ===\n");
    }

    close(fd);
    exit(0);
}

