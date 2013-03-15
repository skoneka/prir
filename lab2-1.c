#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    pid_t pid;
    pid = getpid();
    printf("Moj PID = %d\n", pid);
    //signal(SIG_STOP);
    kill(pid,SIGSTOP);

    return 0;
}
