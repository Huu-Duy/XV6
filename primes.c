#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int pfd[2]) __attribute__((noreturn));

// Moi stage trong pipeline doc tu pfd[0], in so nguyen to,
// roi chuyen cac so khong chia het cho no sang stage ke tiep.
void primes(int pfd[2])
{
    int head, cur, nb;
    int ch[2];   // pipe sang stage ke tiep
    int cpid;

    close(pfd[1]);   // stage nay chi doc, khong ghi vao pipe nay

    // Doc so dau tien - do la so nguyen to cua stage nay
    nb = read(pfd[0], &head, 4);
    if (nb == 0) {
        // Khong con so nao: ket thuc pipeline
        close(pfd[0]);
        exit(0);
    }
    if (nb != 4) {
        close(pfd[0]);
        exit(1);
    }

    printf("prime %d\n", head);

    // Tao pipe moi cho stage ke tiep
    if (pipe(ch) < 0) {
        fprintf(2, "pipe error\n");
        close(pfd[0]);
        exit(1);
    }

    cpid = fork();
    if (cpid < 0) {
        fprintf(2, "fork error\n");
        close(pfd[0]);
        close(ch[0]);
        close(ch[1]);
        exit(1);
    }

    if (cpid == 0) {
        // Tien trinh con: xu ly stage tiep theo
        close(pfd[0]);
        primes(ch);
    }

    // Tien trinh cha: loc va gui so khong phai boi so nguyen to hien tai
    close(ch[0]);

    while ((nb = read(pfd[0], &cur, 4)) == 4) {
        if (cur % head != 0) {
            if (write(ch[1], &cur, 4) != 4) {
                close(pfd[0]);
                close(ch[1]);
                wait(0);
                exit(1);
            }
        }
    }

    close(pfd[0]);
    close(ch[1]);   // bao hieu EOF cho stage ke tiep

    wait(0);   // cho stage ke tiep va cac chau ket thuc
    exit(0);
}

int main()
{
    int src[2];
    int i, pid;

    if (pipe(src) < 0) {
        fprintf(2, "pipe error\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
        close(src[0]);
        close(src[1]);
        exit(1);
    }

    if (pid == 0) {
        // Tien trinh con: chay toan bo pipeline
        primes(src);
    }

    // Tien trinh cha: dua so tu 2 den 280 vao pipe
    close(src[0]);

    for (i = 2; i <= 280; i++) {
        if (write(src[1], &i, 4) != 4) {
            fprintf(2, "write error\n");
            close(src[1]);
            wait(0);
            exit(1);
        }
    }

    close(src[1]);
    wait(0);
    exit(0);
}
    