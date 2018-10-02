#include "csapp.h"

void *test(void *vargp) {
    char *b;
    b = (char*)malloc(4 * sizeof(char));
    strcpy(b, "tes");
    return (void *)b;
}

int main(int argc, char const *argv[])
{
	char *a;
    // void *b;
    pthread_t tid;
	// a = test();
    
    Pthread_create(&tid, NULL, test, NULL);
    // pthread_join(tid, &b);
    // printf("%d\n", (int)*b);
    Pthread_join(tid, (void **)&a);
    printf("%s\n", a);

	return 0;
}