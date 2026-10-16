#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


int flag_a = 0;
int flag_s = 0;


uint64
du(char *path, int is_root)
{
   char buf[512], *p;
   int fd;
   struct dirent de;
   struct stat st;
   uint64 total_size = 0;


   if((fd = open(path, O_RDONLY)) < 0){
       fprintf(2, "du: cannot open %s\n", path);
       return 0;
   }


   if(fstat(fd, &st) < 0){
       fprintf(2, "du: cannot stat %s\n", path);
       close(fd);
       return 0;
   }


   if(st.type == T_FILE){
       total_size = st.size;
       close(fd);


       if(flag_a && !flag_s){
           printf("%ld\t%s\n", total_size, path);
       }
       return total_size;
   }


   if(st.type == T_DIR){
       if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
           fprintf(2, "du: path too long\n");
           close(fd);
           return 0;
       }


       strcpy(buf, path);
       p = buf + strlen(buf);
       *p++ = '/';


       while(read(fd, &de, sizeof(de)) == sizeof(de)){
           if(de.inum == 0)
               continue;


           if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
               continue;


           memmove(p, de.name, DIRSIZ);
           p[DIRSIZ] = 0;


           total_size += du(buf, 0);
       }


       close(fd);


       if(!flag_s || is_root){
           printf("%ld\t%s\n", total_size, path);
       }


       return total_size;
   }


   close(fd);
   return 0;
}


int
main(int argc, char *argv[])
{
   char *path = 0;
   int i;


   for(i = 1; i < argc; i++){
       if(strcmp(argv[i], "-a") == 0){
           flag_a = 1;
       } else if(strcmp(argv[i], "-s") == 0){
           flag_s = 1;
       } else if(argv[i][0] == '-'){
           fprintf(2, "usage: du [path] [-a] [-s]\n");
           exit(1);
       } else {
           path = argv[i];
       }
   }


   if(path == 0)
       path = ".";


   du(path, 1);
   exit(0);
}
