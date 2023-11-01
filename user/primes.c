#include "kernel/types.h" 
#include "kernel/stat.h"
#include "user/user.h" // 系统函数

int
sub_process(int p_0){
    int buf[2];
    read(p_0, buf, 2);
    int prime_from_left = *buf;
    printf("prime %d\n", prime_from_left);
    if(prime_from_left >= 31){
        return 0;
    }

    int new_p[2];
    pipe(new_p);

    int pid = fork();
    if(pid < 0){
        printf("error\n");
        return 1;
    }

    int status;

    if(pid == 0){
        close(new_p[1]);
        close(p_0);
        if(prime_from_left > 0){
            if(sub_process(new_p[0])) return 1; // 无限开子进程的问题,如何限制？？？
            return 0;
        }
    }
    else{
        int buf[2];

        close(new_p[0]);
        while(read(p_0, buf, 2)){
            int number_from_left = *buf;
            // printf("%d ", *buf);

            int buffer[2];
            buffer[0] = number_from_left;
            if(number_from_left % prime_from_left > 0) write(new_p[1], buffer, 2);       
        }
        close(p_0);
        close(new_p[1]);

        wait(&status);
    }
    
    return 0;
}

int 
main (int argc, char * argv[]){
    int p[2];
    pipe(p);

    int status;
    int pid = fork();
    if(pid < 0){
        printf("error\n");
        exit(1);
    }

    if(pid == 0){  
        close(p[1]);
        if(sub_process(p[0])) exit(1);
        exit(0);
    }
    else{
        close(p[0]); // 只用写端
        for(int i = 2; i <= 35; i ++ ){
            int buf[2];
            buf[0] = i;
            write(p[1], buf, 2); // 直接传入int
        }
        close(p[1]);
        
        wait(&status); // 如何做到等待所有子程序回归
    }

    exit(0);
}