#include "csapp.h"

int N = 5;
sem_t sem, wmutex;

int write_total_cnt;
int read_total_cnt;

void *reader(void *v)
{
    printf("reader start\n");
    while (1) {
        P(&sem);

        read_total_cnt++;

        V(&sem);
    }
}

void *writer(void *v)
{
    int i;
    printf("writer start\n");
    write_total_cnt++;
    while (1) {
        P(&wmutex);
        for (i=0; i<N; i++)
            P(&sem);
        V(&wmutex);

        write_total_cnt++;

        for (i=0; i<N; i++)
            V(&sem);
    }
}

void handler(int sig)
{
    printf("%d %d\n", read_total_cnt, write_total_cnt);
}

int main(int argc, char const *argv[])
{
Sem_init(&wmutex, 0, 1);
Sem_init(&sem, 0, N);
Signal(SIGALRM, handler);
Alarm(2);
    pthread_t tid1, tid2, tid3, tid4, tid5, tid6;
    Pthread_create(&tid1, NULL, reader, NULL);
    Pthread_create(&tid2, NULL, writer, NULL);
    Pthread_join(tid1, NULL);
    Pthread_join(tid2, NULL);
    return 0;
}