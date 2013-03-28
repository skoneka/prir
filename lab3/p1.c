//Artur Skonecki, Łukasz Załęsk

// gcc p1.c
// ./a.out [magiczna liczba oznaczajaca liczbe losowanych liczb do sumowania]

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


//kiedy segfaultuje to zmienic wartosc AKEY
//#define AKEY 10001112351231232343
#define ASIZE 4

int handle ;
int handleb ;

int handlec ;


int *zakresy  ;

int *ans  ;
int *ptr  ;

int child_num = -1;

int w[] = { 1 , 2 , 3, 4};

int dosumuj()
{
	int a = zakresy[child_num*2];
	int b = zakresy[child_num*2 +1];
	int i =0;

	int suma = 0;
	for(i = a;i<=b;i++)
	{
		suma += ptr[i];
	}
	
	return suma;
}

void zapisz(int suma)
{
	ans[child_num] = suma;	
}

void catcher( int sig ) {

    printf( "inside catcher() function\n" );


    printf("gut %d\n", child_num);

    /*int n, m=0;
    FILE *f = fopen("config.conf", "r");
    fscanf(f, "%d", &n);
    int i;
    for (i = 1; i <= n; i++) m+=i;
    printf("\n%d\n", m);*/
    int wyn = dosumuj();
    zapisz(wyn);
}

void wait_child() {
    sigset_t sigset;

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
}


int main( int argc, char **argv) {
	int AKEY = rand();
	//seglen - liczba procesow dzieci
	if (argc < 2) {
		printf("podaj liczbe procesow dzieci\n");
		return 2;
	}
	int seglen = atoi(argv[1]);
	handle = shmget(AKEY, seglen*2*sizeof(int), IPC_CREAT|0666);
	printf("%d\n", handle);
	handleb = shmget(AKEY+1, seglen*2*sizeof(int), IPC_CREAT|0666);
	zakresy = shmat(handleb, NULL, 0);

	handlec = shmget(AKEY+2, seglen*sizeof(int), IPC_CREAT|0666);
	ans = shmat(handlec, NULL, 0);
	ptr = shmat(handle, NULL, 0);
	if(ptr != NULL)
		printf("blah\n");

	int i;
	int tab_child[1000];
	for( i=0; i< seglen; i++) {
		int pid = fork();
		if (!pid) {
		    child_num = i;
		    printf("pid %d\n", pid);
		    printf("czekam dziecko\n");
	            wait_child();
		    while(1)
		       pause();
			
		    printf("gut\n");
			break;
		}
                else {
			tab_child[i] = pid;
			printf("rodzic\n");
		}
	}


	int dlvec;
	
	//memcpy(ptr, w, ASIZE*sizeof(int));
	//for( i=0; i< ASIZE; i++) {
//		printf("%d\n", (int)ptr[i]);
		
//	}
	int dlzakr = 1;
	int resz = 0;
	for(i = 0;i <seglen;i+=1)
	{
		zakresy[2*i] = resz;
		resz += dlzakr;
		zakresy[2*i + 1] = resz;
		resz += dlzakr;
		printf("%d %d\n", zakresy[2*i], zakresy[2*i + 1]);	
	}

	for( i=0; i< seglen*2; i++) {
		ptr[i] = i;
		printf("%d, ", ptr[i]);
		
	}
	printf("\n");
	

//////////

	for (i=0; i<seglen; i++) {
		kill(tab_child[i], SIGHUP);
	}
	
//czekamy
	wait(0);
	int suma = 0;
	for(i = 0;i<seglen;i++)
	{
		printf("%d, ",ans[i]);
		suma += ans[i];
	}
	printf("suma: %d\n", suma);
	
	shmdt(ptr);
	shmdt(zakresy);
	shmdt(ans);
	shmctl(handle, SHM_UNLOCK, 0); 
	shmctl(handleb, SHM_UNLOCK, 0); 
	shmctl(handlec, SHM_UNLOCK, 0); 

}
