#include "csapp.h"

int main(int argc, char **argv, char **envp){
	struct stat stat;
	int fd;
	char *bufp;

	if(argc !=2 ){
		printf("usage: %s filename", argv[0]);
	}


	fd = Open(argv[1], O_RDONLY, 0);
	fstat(fd, &stat);

	bufp = Mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	Write(1, bufp, stat.st_size);

	return 0;
}
