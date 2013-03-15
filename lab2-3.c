#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>



sigset_t sigset;

void catcher( int sig ) {

    printf( "inside catcher() function\n" );
    int n, m=0;
    FILE *f = fopen("config.conf", "r");
    fscanf(f, "%d", &n);
    int i;
    for (i = 1; i <= n; i++) m+=i;
    printf("\n%d\n", m);
}

int main()
{
    pid_t pid;
    pid = getpid();

    struct sigaction sigact, old_sigact;

   /*
    * Set up an American National Standard C style signal handler
    * by setting the signal mask to the empty signal set and
    * using the do-not-defer signal, and reset the signal handler
    * to the SIG_DFL signal flag options.
    */

    sigemptyset( &sigact.sa_mask );
    sigact.sa_flags = 0;
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_handler = catcher;
    sigaddset(&sigset, SIGHUP);
    sigaction( SIGHUP, &sigact, NULL );
    sigact.sa_mask = sigset;

    printf("Moj PID = %d\n", pid);


    printf( "raise SIGHUP signal\n" );
    kill( getpid(), SIGHUP );
    //signal(SIG_STOP);

    while(1)
       pause();

    return 0;

}
