/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych
 * SYSV IPC (semafory i pamiec dzielona)
 *
 * main.c -- proces startujacy obliczenia i zbierajacy wyniki.
 *
 * $Id: main.c,v 1.3 1995/05/25 15:26:24 ela16pbl Exp $
 *********************************************************************/
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defsIPC.h"


int		*size;		/* rozmiar zadania	*/
double (*A)[MAXSIZE][MAXSIZE];	/* macierz ukladu	*/
double (*b)[MAXSIZE];		/* wektor b		*/
double (*xo)[MAXSIZE];		/* wektor x		*/

int	*local_stop;	   /* Tablica znacznikow osiagniecia	*/
			   /* lokalnego testu stopu		*/
int	lid;

int	Aid, xid, sid, bid;	/* identyfikatory obszarow pamieci	*/
int	sem;			/* i semafora				*/

int	p;			/* liczba procesow	*/

int
main(int argc, char **argv)
{
	if (argc < 2)
		p = 2;
	else
		p = atoi(argv[1]);

	getmem();
	readA();
	readb();
	work();
	freemem();
	return 0;
}

/*
 * Alokacja pamieci dzielonej i semafora
 */
void
getmem()
{
	Aid = shmget( (key_t) AKEY, ASIZE, PERM|IPC_CREAT);
	xid = shmget( (key_t) XKEY, XSIZE, PERM|IPC_CREAT);
	bid = shmget( (key_t) BKEY, XSIZE, PERM|IPC_CREAT);
	sid = shmget( (key_t) SKEY, sizeof(int), PERM|IPC_CREAT);
	lid = shmget( (key_t) LKEY, p*sizeof(int), PERM|IPC_CREAT);

	xo = (void*) shmat(xid, 0, 0);
	A = (void*) shmat(Aid,0,0);
	size = (int*) shmat(sid,0,0);
	b = (void*) shmat(bid,0,0);
	local_stop = (int*) shmat(lid,0,0);

	/*	Nikt jeszcze nie skonczyl obliczen ;-) */
	memset(local_stop, 0, p*sizeof(int));

	sem = semget( (key_t) SEMKEY, 2, PERM|IPC_CREAT);
	semcall(sem, 1, READ_WRITE);
	semcall(sem, 1, STOP_TEST);
}

/*
 * Zwolnienie pamieci dzielonej i semafora
 */
void
freemem()
{
	shmdt((void*)A);
	shmdt((void*)xo);
	shmdt((void*)size);
	shmdt((void*)b);
	shmdt((void*)local_stop);

	shmctl(Aid, IPC_RMID, 0);
	shmctl(xid, IPC_RMID, 0);
	shmctl(sid, IPC_RMID, 0);
	shmctl(bid, IPC_RMID, 0);
	shmctl(lid, IPC_RMID, 0);
	semctl(sem, 0, IPC_RMID, 0);
}

/*
 * Wczytanie macierzy A
 */
void
readA()
{
	int i,j;
	FILE *afile;

	afile = fopen("A.in", "r");

	fscanf(afile,"%d", size);

	for (i=0; i<*size; i++) {
		for (j=0; j<*size; j++) {
			fscanf(afile,"%lf", &(*A)[i][j]);
		}
	}
	fclose(afile);
}

/*
 * Wczytanie wektora b
 */
void
readb()
{
	int i;
	FILE *bfile;

	bfile = fopen("b.in", "r");

	fscanf(bfile,"%d", size);

	for (i=0; i<*size; i++) {
		fscanf(bfile,"%lf", &(*b)[i]);
	}
	fclose(bfile);
}

/*
 * Podzial problemu i uruchomienie procesow obliczeniowych
 */
void
work()
{
	int i, k;
	int rest;
	int beg, end;
	char arg[4][10];
	int status;

	/* Podzial zadania i uruchomienie procesow roboczych */
#ifdef DEBUG
	printf("size = %d\n", *size);
#endif
	k = *size/p;
	rest = *size - k*p;

	for (i=0; i<rest; i++) {
		beg = i*(k+1);
		end = (i+1)*(k+1)-1;
#ifdef DEBUG
		printf("p = %d <%d> (%d,%d)\n", i, end-beg+1, beg, end);
#endif
		sprintf(arg[1], "%d", p);
		sprintf(arg[2], "%d", i);
		sprintf(arg[3], "%d", beg);
		sprintf(arg[4], "%d", end);
		if (fork()==0) {
			execl("./work", "./work", arg[1], arg[2], arg[3], arg[4], NULL);
		}
	}

	for (i=0; i<p-rest; i++) {
		beg = rest*(k+1)+i*k;
		end = rest*(k+1)+(i+1)*k-1;
#ifdef DEBUG
		printf("p = %d <%d> (%d,%d)\n", i+rest, end-beg+1, beg, end);
#endif
		sprintf(arg[1], "%d", p);
		sprintf(arg[2], "%d", i+rest);
		sprintf(arg[3], "%d", beg);
		sprintf(arg[4], "%d", end);
		if (fork()==0) {
			execl("./work", "./work", arg[1], arg[2], arg[3], arg[4], NULL);
		}
	}

	/* oczekiwanie na zakonczenie obliczen */
	for (i = 0; i<p; i++) {
		wait(&status);
#ifdef DEBUG
		printf("Finished %d\n", i);
#endif
	}

	printf("size = %d ==>\n", *size);
	for (i=0; i < *size; i++) {
		printf("x%.2d ==> %f\n", i, (*xo)[i]);
	}
}
