#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUF_SIZE 512
#define MAX_DEPTH 512

struct dirent_info {
  char name[DIRSIZ + 1];
  short type;
};

// Mảng lưu trạng thái đường kẻ đứng cho từng độ sâu
int has_sibling[MAX_DEPTH];

void print_indent(int level, int is_last) {
  for (int i = 0; i < level; i++) {
    if (has_sibling[i]) {
      printf("│   ");
    } else {
      printf("    ");
    }
  }
  if (is_last) {
    printf("└── ");
  } else {
    printf("├── ");
  }
}

void tree(char *path, int level, int max_depth, int only_dir) {
  if (max_depth >= 0 && level >= max_depth) {
    return;
  }

  int fd;
  struct dirent de;
  struct stat st;
  char buf[BUF_SIZE];

  if ((fd = open(path, O_RDONLY)) < 0) {
    printf("tree: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    printf("tree: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  int capacity = st.size / sizeof(de) + 1;
  struct dirent_info *list = malloc(capacity * sizeof(*list));
  if (list == 0) {
    printf("tree: cannot allocate memory\n");
    close(fd);
    return;
  }
  int count = 0;

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    char name[DIRSIZ + 1];
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = 0;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    if (strlen(path) + 1 + strlen(name) + 1 > sizeof(buf)) {
      printf("tree: path too long\n");
      continue;
    }

    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p++ = '/';
    memmove(p, name, strlen(name) + 1);

    int sub_fd = open(buf, O_RDONLY);
    if (sub_fd < 0)
      continue;

    if (fstat(sub_fd, &st) < 0) {
      close(sub_fd);
      continue;
    }
    close(sub_fd);

    if (only_dir && st.type != T_DIR)
      continue;

    memmove(list[count].name, name, sizeof(name));
    list[count].type = st.type;
    count++;
  }
  close(fd);

  for (int i = 0; i < count; i++) {
    int is_last = (i == count - 1);
    has_sibling[level] = !is_last;

    print_indent(level, is_last);
    printf("%s\n", list[i].name);

    if (list[i].type == T_DIR) {
      strcpy(buf, path);
      char *p = buf + strlen(buf);
      *p++ = '/';
      memmove(p, list[i].name, strlen(list[i].name));
      p[strlen(list[i].name)] = 0;

      tree(buf, level + 1, max_depth, only_dir);
    }
  }
  free(list);
}

int main(int argc, char *argv[]) {
  char *path = ".";
  int max_depth = -1;
  int only_dir = 0;
  int path_seen = 0;

  // Xử lý cờ dòng lệnh
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      only_dir = 1;
    } else if (strcmp(argv[i], "-L") == 0) {
      if (i + 1 < argc) {
        max_depth = atoi(argv[++i]);
        if (max_depth < 0) {
          printf("tree: depth must be non-negative\n");
          exit(1);
        }
      } else {
        printf("tree: missing argument to -L\n");
        exit(1);
      }
    } else if (argv[i][0] != '-') {
      if (path_seen) {
        printf("tree: too many paths\n");
        exit(1);
      }
      path = argv[i];
      path_seen = 1;
    } else {
      printf("tree: unknown option %s\n", argv[i]);
      exit(1);
    }
  }

  printf("%s\n", path);
  tree(path, 0, max_depth, only_dir);

  exit(0);
}