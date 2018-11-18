#include "csapp.h"
#define MAX 1024

void handler(){
	printf("string get\n");
	exit(0);
}

int main(int argc, char *argv[], char *envp[]){
	int status;
	pid_t pid;
	char test[MAX];

	//register signal handler
	Signal(SIGALRM, handler);

	if((pid = Fork())==0){
		fgets(test, MAX, stdin);
		printf("%s", test);
		//wake father process
		Kill(getppid(),SIGALRM);
	}else{
		sleep(5);
		//kill son process
		printf("no string get, end.\n");
		Kill(pid,SIGKILL);
		exit(0);
	}
}

// void handler(){
// 	printf("end\n");
// 	exit(0);
// }

// int main(int argc, char *argv[], char *envp[]){
// 	char test[MAX];
// 	Signal(SIGALRM, handler);
// 	alarm(5);//5秒后发送SIGALRM信号给自己
// 	fgets(test, MAX, stdin);
// 	printf("%s", test);
// 	exit(0);
// }