#include "csapp.h"

int main(int argc, char **argv) {
    int clientfd, cnt;
    char *port, *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (cnt = Rio_readlineb(&rio, buf, MAXLINE)) {
        printf("cnt: %d\n", cnt);
        Fputs(buf, stdout);
    }
    Close(clientfd);
    exit(0);
}