#include "kernel/types.h" 
#include "kernel/stat.h"
#include "user/user.h" 
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  char buf[512];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  strcpy(buf, p);
  p = buf;
  return p;
}

void 
find(char * path, const char * name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;
        if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
        
        memmove(p, de.name, sizeof(de.name));
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch(st.type){
        case T_FILE:
            if(!strcmp(fmtname(buf), name)) printf("%s\n", buf);
            break;
        
        case T_DIR:
            if(strlen(buf) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                return;
            }
            find(buf, name);
            break;
        }
    }
    close(fd);
}

int 
main(int argc, char * argv[])
{
    if(argc <= 2){
        printf("file name is needed\n");
        exit(0);
    }

    find(".", argv[2]);
    exit(0);
}