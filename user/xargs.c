#include "kernel/types.h" 
#include "kernel/stat.h"
#include "user/user.h" 
#include "kernel/fcntl.h"
#include "kernel/param.h"

void 
fork_read_loop(int argc, char * argv[])
{
    int pid = fork(), status;

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        char c, *add_argv[MAXARG], *final_argv[MAXARG];
        for(int i = 1; i < argc; i ++ ){
            final_argv[i - 1] = (char *)malloc(MAXARG *sizeof(char));
            int k = -1;
            while(argv[i][++ k]) 
                final_argv[i - 1][k] = argv[i][k];
        }
        
        int fd = 0, u = 0, v = 0;
        for(int i = 0; i < MAXARG; i ++ )
            add_argv[i] = (char *)malloc(MAXARG * sizeof(char));

        int a = read(fd, &c, sizeof(c));
        if(a == 0) exit(1);
        
        while(a == sizeof(c)){
            if(c == ' '){
                u ++ ;
                v = 0;
            }
            else if(c == '\n'){
                for(int i = argc - 1, k = 0; k <= u; i ++ , k ++ ){
                    final_argv[i] = (char *)malloc(MAXARG *sizeof(char));
                    int j = -1;
                    while(add_argv[k][++ j]) 
                        final_argv[i][j] = add_argv[k][j];
                    //final_argv[i][++ j] = '\0';
                }
                /*
                printf("\nadd argv:");
                for(int i = 0; i <= u; i ++ )
                    printf("%s ", add_argv[i]);
                // printf("\nfinal argv:");
                for (int i = 0; i < argc + u; i ++ ) 
                   printf("%s ", final_argv[i]);
                printf("\n");
                */
                exec(final_argv[0], final_argv);
            }
            else{
                add_argv[u][v ++ ] = c;
            }
            a = read(fd, &c, sizeof(c));   
        }
        // printf("\n");
         // 释放分配的内存
        for (int i = 0; i < argc - 1; i++) 
            free(final_argv[i]);
        for (int i = 0; i < MAXARG; i++) 
            free(add_argv[i]);
        exit(0);
    }
    else{
        wait(&status);
        // printf("status: %d\n", status);
        if(status == 1) exit(1);

        fork_read_loop(argc, argv);
    }
}

int 
main(int argc, char * argv[]) // argv只会读到后面？？
{
    fork_read_loop(argc, argv);

    exit(0);
}