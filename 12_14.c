#include "csapp.h"

// void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char const *argv[])
{
    int listenfd, *connfdp, portlen;
    char *port;
    socklen_t clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    portlen = strlen(argv[1]) + 1;
    port = Malloc(portlen * sizeof(char));
    strncpy(port, argv[1], portlen);
    listenfd = Open_listenfd(port);
    while (1) {
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfdp);
    }

    return 0;
}

void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    write(connfd, "get", 3);
    Close(connfd);
    return NULL;
}