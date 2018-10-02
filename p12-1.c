#include "csapp.h"

void *thread(void *vargp);

int main(int argc, char **argv)
{
    int num, i;
    pthread_t *ps;

    if (argc != 2) {
        printf("usage: <num>\n");
    }
    num = atoi(argv[1]);
    ps = (pthread_t *)malloc(num * sizeof(pthread_t));
    for (i = 0; i < num; i++) {
        Pthread_create(&ps[i], NULL, thread, NULL);
    }
    for (i = 0; i < num; i++) {
        Pthread_join(ps[i], NULL);
    }
    exit(0);
}

void *thread(void *vargp)
{
    printf("Hello, little prince\n");
    return NULL;
}