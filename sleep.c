#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
    // Kiem tra loi thieu tham so   
    if (argc < 2) {
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    // Chuyen chuoi sang so nguyen
    int ticks = atoi(argv[1]);

    // Goi system call sleep voi tham so la so tick
    sleep(ticks);
    exit(0);
}
