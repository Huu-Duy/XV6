#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


uint64
du_v1(char *path)
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
       return total_size;
   }


   if(st.type == T_DIR){
       strcpy(buf, path);
       p = buf + strlen(buf);
       *p++ = '/';


       while(read(fd, &de, sizeof(de)) == sizeof(de)){
           if(de.inum == 0)
               continue;

           strcpy(p, de.name);
           total_size += du_v1(buf);
       }
   }


   close(fd);
   printf("%ld\t%s\n", total_size, path);
   return total_size;
}


int
main(int argc, char *argv[])
{
   char *path = (argc < 2) ? "." : argv[1];
   du_v1(path);
   exit(0);
}

