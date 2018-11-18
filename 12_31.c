#include "csapp.h"
#define MAXTHREADS 32

void *sum(void *vargp);

long psum[MAXTHREADS];
long nelems_per_thread;

int main(int argc, char **argv)
{
    long i, nelems, log_nelems, nthreads, result = 0;
    pthread_t tid[MAXTHREADS];
    int myid[MAXTHREADS];

    if (argc != 3) {
        printf("Usage: %s <nthreads> <log_nelems>\n", argv[0]);
        exit(0);
    }
    nthreads = atoi(argv[1]);
    log_nelems = atoi(argv[2]);
    nelems = (1L << log_nelems);
    nelems_per_thread = nelems / nthreads;

    for (i = 0; i < nthreads; i++) {
        myid[i] = i;
        Pthread_create(&tid[i], NULL, sum, &myid[i]);
    }
    for (i = 0; i < nthreads; i++) {
        Pthread_join(tid[i], NULL);
    }
    for (i = 0; i < nthreads; i++) {
        result += psum[i];
    }
    if (result != (nelems * (nelems - 1)) / 2)
        printf("Error: result = %ld\n", result);

    exit(0);
}

void *sum(void *vargp)
{
    int myid = *((int *)vargp);
    long start = myid * nelems_per_thread;
    long end = start + nelems_per_thread;
    long i, sum = 0;

    for (i = start; i < end; i++) {
        sum += i;
    }
    psum[myid] = sum;

    return NULL;
}