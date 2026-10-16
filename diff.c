#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_LINE 512

int readline(int fd, char *buf, int max) {
  int i = 0;
  char c;
  int n;

  while (i < max - 1) {
    n = read(fd, &c, 1);
    if (n < 0) {
      return -1;
    }
    if (n == 0) {
      if (i == 0) {
        return 0;
      }
      break;
    }
    if (c == '\n') {
      break;
    }
    buf[i++] = c;
  }

  buf[i] = '\0';
  return 1;
}

int main(int argc, char *argv[]) {
  char *file1 = 0;
  char *file2 = 0;
  int qflag = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-q") == 0) {
      qflag = 1;
    } else if (file1 == 0) {
      file1 = argv[i];
    } else if (file2 == 0) {
      file2 = argv[i];
    } else {
      fprintf(2, "usage: diff file1 file2 [-q]\n");
      exit(1);
    }
  }

  if (file1 == 0 || file2 == 0) {
    fprintf(2, "usage: diff file1 file2 [-q]\n");
    exit(1);
  }

  int fd1 = open(file1, O_RDONLY);
  if (fd1 < 0) {
    fprintf(2, "diff: cannot open %s\n", file1);
    exit(1);
  }

  int fd2 = open(file2, O_RDONLY);
  if (fd2 < 0) {
    fprintf(2, "diff: cannot open %s\n", file2);
    close(fd1);
    exit(1);
  }

  char buf1[MAX_LINE];
  char buf2[MAX_LINE];
  int line_num = 1;

  while (1) {
    int r1 = readline(fd1, buf1, sizeof(buf1));
    int r2 = readline(fd2, buf2, sizeof(buf2));

    if (r1 < 0 || r2 < 0) {
      fprintf(2, "diff: read error\n");
      close(fd1);
      close(fd2);
      exit(1);
    }

    if (r1 == 0 && r2 == 0) {
      break;
    }

    int differ;
    if (r1 > 0 && r2 > 0) {
      differ = (strcmp(buf1, buf2) != 0);
    } else {
      differ = 1;
    }

    if (differ) {
      if (qflag) {
        printf("diff: files differ\n");
        close(fd1);
        close(fd2);
        exit(0);
      }

      if (r1 > 0) {
        printf("%s:%d: < %s\n", file1, line_num, buf1);
      } else {
        printf("%s:%d: < EOF\n", file1, line_num);
      }

      if (r2 > 0) {
        printf("%s:%d: > %s\n", file2, line_num, buf2);
      } else {
        printf("%s:%d: > EOF\n", file2, line_num);
      }
    }

    line_num++;
  }

  close(fd1);
  close(fd2);
  exit(0);
}
