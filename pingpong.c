#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p_to_c[2]; // Pipe: Cha -> Con
    int c_to_p[2]; // Pipe: Con -> Cha
    char buf;

    // Tao 2 pipe
    if (pipe(p_to_c) < 0 || pipe(c_to_p) < 0) {
        fprintf(2, "pingpong: pipe failed\n");
        exit(1);
    }

    // Tao tien trinh con
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "pingpong: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Tien trinh con 
        close(p_to_c[1]); // Dong dau ghi p_to_c
        close(c_to_p[0]); // Dong dau doc c_to_p

        // Doc 1 byte tu cha
        if (read(p_to_c[0], &buf, 1) == 1) {
            printf("%d: received ping\n", getpid());
        }

        // Gui 1 byte lai cho cha
        write(c_to_p[1], &buf, 1);

        close(p_to_c[0]);
        close(c_to_p[1]);
        exit(0);
    } else {
        // Tien trinh cha
        close(p_to_c[0]); // Dong dau doc p_to_c
        close(c_to_p[1]); // Dong dau ghi c_to_p

        // Gui 1 byte dau tien cho con
        buf = 'x';
        write(p_to_c[1], &buf, 1);

        // Doc 1 byte phan hoi tu con
        if (read(c_to_p[0], &buf, 1) == 1) {
            printf("%d: received pong\n", getpid());
        }

        close(p_to_c[1]);
        close(c_to_p[0]);

        // Cho tien trinh con ket thuc
        wait(0);
        exit(0);
    }
}