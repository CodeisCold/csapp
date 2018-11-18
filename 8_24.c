#include "csapp.h"
#define N 2

// gcc 8.24.c csapp.c -pthread
int main()
{
    char *ro = "hello, world!";
    char s[1024];
    int status, i;
    pid_t pid;

    /* Parent creates N children */
    for (i = 0; i < N; i++)
        if ((pid = Fork()) == 0)  /* Child */
        {
            ro[0] = 'l';
        }

    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status))
        {
            printf("child %d terminated normally with exit status=%d\n",
                pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            int sig = WTERMSIG(status);
            sprintf(s, "child %d terminated by signal %d", pid, sig);
            psignal(sig, s);
            // printf("child %d terminated by signal %d\n", pid, sig);
        }
        else
        {
            printf("child %d terminated abnormally\n", pid);
        }
    }

    /* The only normal termination is if there are no more children */
    if (errno != ECHILD)
        unix_error("waitpid error");

    exit(0);
}