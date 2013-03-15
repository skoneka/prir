#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h> 
#include <errno.h>
pthread_mutex _t infoMutex;
char t_info[1024] = {0};

int getFreeThread(pthread_t *arr, int N) {
	int i = 0;
	while(1) {
		if(i == N) 
			i = 0;
		pthread_mutex_lock(&infoMutex);
		if(t_info[i] == 0) {
			pthread_mutex_unlock(&infoMutex);
			return i;
		}
		pthread_mutex_unlock(&infoMutex);
		i++;
	}
}

typedef struct s {
	double **A;
	double **B;
	int a;
	int b;
	int c;
	int d;
	int id;a
} s_t;

void mn(void *c) {

}

int sum(int thNum, pthread_t *tr, double **A, double **B, int b)
{
	s_t *p = malloc(sizeof(p_t));
	pthread_create(tr[thNum], NULL, mn, p);
}

void mnoz(double**A, int a, int b, double**B, int c, int d, double**C, int N)
{
	
	pthread_t *th = malloc(sizeof(pthread_t) * N);
    int i, j, k;
    double s;
    for(i=0; i<a; i++)
    {
        for(j=0; j<d; j++)
        {

		int t = freet(th, N);
		/*
	    int	s = 0;
            for(k=0; k<b; k++)
            {

                s+=A[i][k]*B[k][j];
            }*/
            //C[i][j] = s;
        }

    }
}

int mnoz

