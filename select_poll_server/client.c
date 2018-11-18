#include "../csapp.h"

int main(int argc, char const *argv[])
{
    int cfd, cnt;
    char line[4096];

    cfd = open_clientfd("localhost", "3333");
    while (1) {
        fgets(line, 4096, stdin);
        printf("line\n");
        cnt = write(cfd, line, strlen(line));
        printf("%d\n", cnt);
    }

    return 0;
}