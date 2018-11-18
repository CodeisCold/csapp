#include "csapp.h"
void command(void);

ssize_t Rio_readlineb_c(rio_t *rp, void *usrbuf, size_t maxlen, int *is_eof);
ssize_t rio_readlineb_c(rio_t *rp, void *usrbuf, size_t maxlen, int *is_eof);
int echo_one_line(rio_t *rp);

typedef struct
{
    int size;
    rio_t **list;
} Rio_list;

void free_rio(rio_t *rp);
void rio_list_init(Rio_list *rio_list);
void rio_list_add(Rio_list *rio_list, int index, rio_t *rio);
void rio_list_remove(Rio_list *rio_list, int index);

int main(int argc, char **argv)
{
    int listenfd, connfd, maxfd, i;
    Rio_list rio_list; 
    rio_t *rio_one;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    fd_set read_set, ready_set;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    maxfd = listenfd;

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(listenfd, &read_set);

    rio_list_init(&rio_list);

    while (1) {
        ready_set = read_set;
        Select(maxfd+1, &ready_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &ready_set))
            command();
        if (FD_ISSET(listenfd, &ready_set)) {
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            rio_one = (rio_t *)malloc(sizeof(rio_t));
            Rio_readinitb(rio_one, connfd);
            rio_list_add(&rio_list, connfd, rio_one);
            FD_SET(connfd, &read_set);
            connfd > maxfd && (maxfd = connfd);
        }
        for (i = listenfd + 1; i <= maxfd; i++) {
            if (FD_ISSET(i, &ready_set)) {
                if (!echo_one_line(rio_list.list[i])) { // 如果遇到 EOF close, 并从 read_set 清除 connfd
                    Close(i);
                    FD_CLR(i, &read_set);
                    rio_list_remove(&rio_list, i);
                }
            }
        }
    }
}

void command(void) {
    char buf[MAXLINE];
    if (!Fgets(buf, MAXLINE, stdin))
        exit(0);
    printf("%s\n", buf);
}

int echo_one_line(rio_t *rp) {
    char buf[MAXLINE];
    int is_eof = 0;
    
    if (Rio_readlineb_c(rp, buf, MAXLINE, &is_eof)) {
        printf("%s\n", buf);
    }
    return !is_eof;
}

ssize_t Rio_readlineb_c(rio_t *rp, void *usrbuf, size_t maxlen, int *is_eof) {
    ssize_t rc;

    if ((rc = rio_readlineb_c(rp, usrbuf, maxlen, is_eof)) < 0)
    unix_error("Rio_readlineb error");
    return rc;
}

ssize_t rio_readlineb_c(rio_t *rp, void *usrbuf, size_t maxlen, int *is_eof) {
    int n, rc;
    char c, *bufp = usrbuf;

    *is_eof = 0;
    for (n = 1; n < maxlen; n++) { 
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        } else if (rc == 0) {
            *is_eof = 1;
            if (n == 1)
            return 0; /* EOF, no data read */
            else
            break;    /* EOF, some data was read */
        } else
            return -1;    /* Error */
    }
    *bufp = 0;
    return n-1;
}

void rio_list_init(Rio_list *rio_list) {
    rio_list->size = 4;
    rio_list->list = (rio_t **)malloc(rio_list->size * sizeof(rio_t *));
}

void rio_list_add(Rio_list *rio_list, int index, rio_t *rio) {
    int i;
    if (index >= rio_list->size) { // 扩容
        int size = rio_list->size;
        while (index >= size) size *= 2; 
        rio_t **new_list = (rio_t **)malloc(size * sizeof(rio_t *));
        for (i = 0; i < rio_list->size; i++) {
            new_list[i] = rio_list->list[i];
        }
        free(rio_list->list);
        rio_list->list = new_list;
        rio_list->size = size;
    }
    rio_list->list[index] = rio;
}

void rio_list_remove(Rio_list *rio_list, int index) {
    free_rio(rio_list->list[index]);
    rio_list->list[index] = NULL;
}

void free_rio(rio_t *rp) {
    free(rp);
}