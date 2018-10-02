#include "csapp.h"

int readcnt;
char lock;
sem_t mutex, w, w2;

int write_total_cnt;
int read_total_cnt;

void *reader(void *v)
{
    printf("reader start\n");
    read_total_cnt++;
    while (1) {
        P(&mutex);
        readcnt++;
        if (readcnt == 1)
            P(&w);
        V(&mutex);

        read_total_cnt++;

        P(&mutex);
        readcnt--;
        if (readcnt == 0) {
            V(&w);
            if (lock) {
                lock = 0;
                V(&w2);
            }
        }
        V(&mutex);
    }
}

void *writer(void *v)
{
    printf("writer start\n");
    write_total_cnt++;
    while (1) {
        P(&w);
        P(&w2);

        write_total_cnt++;

        if (!readcnt) {
            V(&w2);
            lock = 1;
        }
        V(&w);
    }
}

void handler(int sig)
{
    printf("%d %d\n", read_total_cnt, write_total_cnt);
}

int main(int argc, char const *argv[])
{
    Sem_init(&mutex, 0, 1);
Sem_init(&w, 0, 1);
Sem_init(&w2, 0, 1);
Signal(SIGALRM, handler);
Alarm(2);
    pthread_t tid1, tid2, tid3, tid4, tid5, tid6;
    Pthread_create(&tid1, NULL, reader, NULL);
    Pthread_create(&tid2, NULL, reader, NULL);
    Pthread_create(&tid3, NULL, reader, NULL);
    Pthread_create(&tid4, NULL, writer, NULL);
    Pthread_create(&tid5, NULL, writer, NULL);
    Pthread_create(&tid6, NULL, writer, NULL);
    Pthread_join(tid1, NULL);
    Pthread_join(tid2, NULL);
    Pthread_join(tid3, NULL);
    Pthread_join(tid4, NULL);
    Pthread_join(tid5, NULL);
    Pthread_join(tid6, NULL);
    return 0;
}