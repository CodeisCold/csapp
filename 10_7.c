#include "csapp.h"
#ifndef MAXBUF
#define MAXBUF 4
#endif
//maxbuf=8192
int main(int argc, char **argv){
	int n;
	rio_t rio;
	char buf[MAXBUF];

	Rio_readinitb(&rio, STDIN_FILENO);

	while((n = Rio_readnb(&rio, buf,MAXBUF))!=0) {
		 Rio_writen(STDOUT_FILENO, buf, n);
	}
}
