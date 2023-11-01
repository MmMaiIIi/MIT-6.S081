#include "kernel/types.h" 
#include "kernel/stat.h"
#include "user/user.h" // 系统函数

int 
main(int argc, char * argv[]){
	// char buf[2];
	int p_c[2], c_p[2];
	pipe(p_c);
	pipe(c_p);

	int pid = fork();
	if(pid < 0){
		printf("error");
		exit(1);
	}

	if(pid == 0){
		int m = getpid();
		close(p_c[1]);
		// printf("%d\n", p_c[1]);
		int buf[2];
		read(p_c[0], buf, 2);
		close(p_c[0]);
		printf("%d: received ping\n%d\n", m, *buf);

		write(c_p[1], "#", 1);
		close(c_p[1]);
		exit(0);
	}
	else{
		int m = getpid();
		close(c_p[1]);

		int buf[2];
		buf[0] = 10;
		write(p_c[1], buf, 2);
		close(p_c[1]);
		// printf("%d\n", p_c[1]);
		read(c_p[0], buf, 1);
		close(c_p[0]);
		printf("%d: received pong\n", m);

		exit(0);
	}
}