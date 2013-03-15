/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych.
 * SYSV IPC (semafory i pamiec dzielona)
 *
 * work.c -- proces obliczeniowy.
 *
 * $Id: work.c,v 1.6 1995/05/26 06:31:22 ela16pbl Exp $
 *********************************************************************/
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "defsIPC.h"

#define delta_x 0.01		/* dokladnosc obliczen		*/

#define MAXITER 500		/* maksymalna liczba iteracji	*/

int	*size;			/* rozmiar zadania 		*/
double (*A)[MAXSIZE][MAXSIZE];	/* macierz ukladu rownan	*/
double (*xo)[MAXSIZE];		/* wektor x			*/
double (*b)[MAXSIZE];		/* wektor b			*/

int	*local_stop;		/* znaczniki osiagniecia lokalnego
				   testu stopu			*/

double D[MAXSIZE];		/* macierz diagonalna D		*/
double xl[MAXSIZE];		/* obliczany kawalek wektora x	*/
double x[MAXSIZE];		/* lokalna kopia wektora x
				   z poprzedniej iteracji	*/

int	sem;			/* semafory do synchronizacji
				   zapisu i odczytu wektora x
				   i znacznikow stopu lokalnego	*/

int	beg, end;		/* fragment problemu rozwiazywany
				   w biezacym zadaniu		*/

int	p;			/* liczba procesow		*/
int	n;			/* numer biezacego procesu	*/

int
main(int argc, char **argv)
{

	p = atoi(argv[1]);	/* liczba procesow */
	n = atoi(argv[2]);	/* numer procesu */
	beg = atoi(argv[3]);	/* poczatek fragmentu x */
	end = atoi(argv[4]);	/* koniec fragmentu x */

#ifdef DEBUG
	printf("Proces %d (%d,%d)\n", n, beg, end);
#endif

	getmem();
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
int		Aid, xid, sid, bid, lid;

	Aid = shmget( (key_t) AKEY, ASIZE, PERM|IPC_CREAT);
	xid = shmget( (key_t) XKEY, XSIZE, PERM|IPC_CREAT);
	bid = shmget( (key_t) BKEY, XSIZE, PERM|IPC_CREAT);
	sid = shmget( (key_t) SKEY, sizeof(int), PERM|IPC_CREAT);
	lid = shmget( (key_t) LKEY, p*sizeof(int), PERM|IPC_CREAT);


	sem = semget( (key_t) SEMKEY, 2, PERM|IPC_CREAT);

	size = (int*)shmat(sid, 0, 0);
	A = (void*)shmat(Aid, 0, 0);
	b = (void*)shmat(bid, 0, 0);
	xo = (void*)shmat(xid, 0, 0);
	local_stop = (int*)shmat(lid, 0, 0);
}

/*
 * Zwolnienie pamieci dzielonej i semafora
 */
void
freemem()
{
	shmdt((void*)size);
	shmdt((void*)A);
	shmdt((void*)b);
	shmdt((void*)xo);
	shmdt((void*)local_stop);
}

/*
 * Obliczenia
 */
void
work()
{
	int i,j,k;
	int	koniec;

	for (i = beg; i<=end; i++) {
		D[i] = 1/(*A)[i][i];
	}

	for(k=0;k<MAXITER;k++) {
		/* x_k = A_k * x^{i-1} */
		for (i = beg; i<=end; i++) {
			xl[i] = 0;
			for (j = 0; j<*size; j++) {
				xl[i] += (*A)[i][j] * x[j];
			}
		}

		/* x_k = x_k - b_k */
		for (i = beg; i<=end; i++) {
			xl[i] -= (*b)[i];
		}

		/* x_k = D_k * x_k */
		for (i = beg; i<=end; i++) {
			xl[i] *= D[i];
		}

		/* x_k^{i} = x_k^{i-1} - x_k */
		for (i = beg; i<=end; i++) {
			xl[i] = x[i] - xl[i];
		}

		/* Zapisanie nowej wersji swojego kawalka x */
		semcall(sem, -1, READ_WRITE);
		memcpy( &(*xo)[beg], &xl[beg], sizeof(double)*(end-beg+1));
		semcall(sem, 1, READ_WRITE);

		/* Test stopu -- lokalny.
		   Sprawdzenie zmiany lokalnego kawalka wektora x */
		koniec=1;
		for (i = beg; i<=end; i++) {
			if ( fabs(x[i] - xl[i]) > delta_x) {
				semcall(sem, -1, STOP_TEST);
				local_stop[n] = 0;		/* Test stopu nie osiagniety... */
				semcall(sem, 1, STOP_TEST);
				koniec=0;
				break;					/* ...dalej */
			}
		}

		/* Odczytanie nowego wektora	*/
		semcall(sem, -1, READ_WRITE);
		memcpy( &x[0], &(*xo)[0], sizeof(double)*(*size));
		semcall(sem, 1, READ_WRITE);

		/* Jesli osiagniety zostal lokalny test stopu to
		   sprawdzenie czy inne procesy tez osiagnely test stopu.
		   Jesli tak -- koniec obliczen -- znaleziono
		   rozwiazanie.						*/
		if (koniec) {
			semcall(sem, -1, STOP_TEST);
			local_stop[n] = 1;
			for (i = 0; i<p; i++) {
				if (!local_stop[i]) {	/* Nie wszystkie procesy	*/
					koniec = 0;	/* osiagnely lokalny test	*/
					break;		/* stopu -- dalej		*/
				}
			}
			semcall(sem, 1, STOP_TEST);
		}

		if (koniec) {		/* Wszystkie procesy osiagnely test stopu	*/
			break;			/* Naprawde _KONIEC_!			*/
		}
	}

#ifdef DEBUG
	printf("End of %d after %d iterations\n", n, k);
#endif
}
