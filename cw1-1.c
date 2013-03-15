#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    pid = getpid();
    printf("Moj PID = %d\n", pid);

    return 0;
}
