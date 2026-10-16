#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUF_SIZE 512

int main(int argc, char *argv[]) { 
  int fd_src, fd_dst, openFlags; 
  char buf[BUF_SIZE]; 
  int n; 

  if (argc != 3) { 
    fprintf(2, "usage: cp src dst\n");
    exit(1);
  }

  fd_src = open(argv[1], O_RDONLY); 
  if (fd_src < 0) { 
    fprintf(2, "cp: cannot open %s\n", argv[1]);
    exit(1);
  }

  openFlags = O_CREATE | O_WRONLY | O_TRUNC; 
  fd_dst = open(argv[2], openFlags);
  if (fd_dst < 0) {
    fprintf(2, "cp: cannot open or create %s\n", argv[2]);
    close(fd_src);
    exit(1);
  }

  while ((n = read(fd_src, buf, BUF_SIZE)) > 0) { 
    if (write(fd_dst, buf, n) != n) {
      fprintf(2, "cp: write error to %s\n", argv[2]);
      close(fd_src);
      close(fd_dst);
      exit(1);
    }
  }


  if (n < 0) { 
    fprintf(2, "cp: read error from %s\n", argv[1]);
    close(fd_src);
    close(fd_dst);
    exit(1);
  }

  close(fd_src);
  close(fd_dst);

  exit(0);
}