/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych.
 * Watki (Solaris threads) - zmienione
 *
 * Watki POSiX (pthreads) - P.Gronek XII.2003
 *
 * workW.c -- procedura watku obliczeniowego.
 *
 *
 *********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include <sys/types.h>
/* #include <sys/processor.h>  - Solaris */
/* #include <sys/procset.h>  - Solaris */

#include "defsW.h"

#define delta_x 1e-9                    /* dokladnosc obliczen          */

#define MAXITER 1000                    /* maksymalna liczba iteracji   */

extern int    size;                     /* rozmiar zadania              */
extern double A[MAXSIZE][MAXSIZE];      /* macierz ukladu rownan        */
extern double xo[MAXSIZE];              /* wektor x                     */
extern double b[MAXSIZE];               /* wektor b                     */

extern int*   local_stop;               /* znaczniki osiagniecia        */
                                        /*    lokalnego testu stopu     */
extern int    p;                        /* liczba watkow                */

extern pthread_mutex_t read_write;
extern pthread_mutex_t stop_test;

extern int num_processors;              /* liczba dostepnych procesorow */
extern int processor[64];

double D[MAXSIZE];                      /* macierz diagonalna D         */
double xl[MAXSIZE];                     /* obliczany kawalek wektora x  */

/*
 * Obliczenia
 */
void*
work(void* th_num)
{
        double x[MAXSIZE];                 /* lokalna kopia wektora x */
        int n;                             /* Numer watku             */
        int k, rest;
        int i,j;
        int     koniec;
        int     beg, end;
        double diff;
        int pold, pno;           /* numer procesora         */
        int status;

        memcpy(&n, th_num, sizeof(int));   /* Numer biezacego watku  ...*/

        pno = n % num_processors;
        /*status = processor_bind(P_LWPID, P_MYID, processor[pno], NULL); - Solaris */
        /*status = processor_bind(P_LWPID, P_MYID, processor[pno], &pold); - Solaris */
        status = 1;
#ifdef DEBUG
        printf ("Thread %d bound to processor %d (status %d)\n",
                        n, processor[pno], status);
#endif

        memset(x, 0, sizeof(double)*size);

        k = size/p;
        rest = size - k*p;

        if (n<rest) {                      /* ... i wynikajacy z niego */
                beg = n*(k+1);             /* kawalek problemu do obrobki */
                end = (n+1)*(k+1)-1;
        } else {
                beg = rest*(k+1)+(n-rest)*k;
                end = rest*(k+1)+(n-rest+1)*k-1;
        }

#ifdef DEBUG
        printf("size = %d p = %d)\n", size, p);
        printf("Thread %d (%d,%d)\n", n, beg, end);
#endif

        for (i = beg; i<=end; i++) {
                D[i] = 1/A[i][i];
        }

        for(k=0;k<MAXITER;k++) {
                /* x_k = A_k * x^{i-1} */
                for (i = beg; i<=end; i++) {
                        xl[i] = 0;
                        for (j = 0; j<size; j++) {
                                xl[i] += A[i][j] * x[j];
                        }
                }

                /* x_k = x_k - b_k */
                for (i = beg; i<=end; i++) {
                        xl[i] -= b[i];
                }

                /* x_k = D_k * x_k */
                for (i = beg; i<=end; i++) {
                        xl[i] *= D[i];
                }

                /* x_k^{i} = x_k^{i-1} - x_k */
                for (i = beg; i<=end; i++) {
                        xl[i] = x[i] - xl[i];
                        /*printf("%d -> %f ", n, x[i]);*/
                }
                        /*printf("\n");*/

                /* Zapisanie nowej wersji swojego kawalka x */
                pthread_mutex_lock(&read_write);
                memcpy( &xo[beg], &xl[beg], sizeof(double)*(end-beg+1));
                pthread_mutex_unlock(&read_write);

                /* Test stopu -- lokalny.
                   Sprawdzenie zmiany lokalnego kawalka wektora x */
                koniec=1;
                for (i = beg; i<=end; i++) {
                        diff = fabs(x[i] - xl[i]);
                        if (diff > delta_x) {
                                pthread_mutex_lock(&stop_test);
                                local_stop[n] = 0;    /* Test stopu */
                                                      /* nie osiagniety... */
                                pthread_mutex_unlock(&stop_test);
                                koniec=0;
                                break;                /* ...dalej */
                        }
                }

                /* Odczytanie nowego wektora    */
                pthread_mutex_lock(&read_write);
                memcpy( &x[0], &xo[0], sizeof(double)*size);
                pthread_mutex_unlock(&read_write);

                /* Jesli osiagniety zostal lokalny test stopu to
                   sprawdzenie czy inne watki tez osiagnely test stopu.
                   Jesli tak -- koniec obliczen -- znaleziono
                   rozwiazanie.                                                                                 */
                if (koniec) {
                        pthread_mutex_lock(&stop_test);
                        local_stop[n] = 1;
                        for (i = 0; i<p; i++) {
                                if (!local_stop[i]) {/* Nie wszystkie watki */
                                        koniec = 0;  /* osiagnely lokalny  */
                                                     /*         test       */
                                        break;       /* stopu -- dalej     */
                                }
                        }
                        pthread_mutex_unlock(&stop_test);
                }

                if (koniec) {      /* Wszystkie watki osiagnely test stopu */
                        break;                  /* Naprawde _KONIEC_!      */
                }
        }

#ifdef DEBUG
        printf("End of %d after %d iterations\n", n, k);
#endif
        pthread_exit(NULL);
        return NULL;
}
