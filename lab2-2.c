#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

sigset_t sigset;
void check_mask( int sig, char *signame ) {

    //sigset_t sigset;

    //sigprocmask( SIG_BLOCK, NULL, &sigset );
    if( sigismember( &sigset, sig ) )
        printf( "the %s signal is blocked\n", signame );
    else
        printf( "the %s signal is unblocked\n", signame );
}

void catcher( int sig ) {

    printf( "inside catcher() function\n" );
    check_mask( SIGUSR1, "SIGUSR1" );
    check_mask( SIGUSR2, "SIGUSR2" );
    check_mask( SIGSTOP, "SIGSTOP" );
    check_mask( SIGTSTP, "SIGTSTP" );
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
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGSTOP);
    sigaddset(&sigset, SIGTSTP);
//sigact.sa_mask = SIG_USR1 | S
    sigaction( SIGUSR1, &sigact, NULL );
    sigaction( SIGUSR2, &sigact, NULL );
    sigaction( SIGSTOP, &sigact, NULL );
    sigaction( SIGTSTP, &sigact, NULL );
    sigact.sa_mask = sigset;

    printf("Moj PID = %d\n", pid);


    printf( "raise SIGUSR1 signal\n" );
    kill( getpid(), SIGUSR1 );
    kill( getpid(), SIGSTOP );
    //signal(SIG_STOP);
    while(1)
       pause();

    return 0;
}
