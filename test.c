#include "csapp.h"

int main(int argc, char const *argv[])
{
	int **t;
    int *test;
    
    t = (int **)malloc(sizeof(int *));
    t[0] = (int *)malloc(sizeof(int));
    test = t[0];
    *(t[0]) = 10;
    free(t);
    printf("%d\n", *test);

	return 0;
}