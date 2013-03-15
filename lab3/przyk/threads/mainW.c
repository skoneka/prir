/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazywanie ukladu rownan liniowych
 *                A x = b
 * gdzie b,x naleza do R^n a A jest macierza n x n
 *
 *
 * Wykorzystywane narzedzie:
 *
 * Watki systemu Solaris (threads) - zmienione
 *
 * Watki POSIX (pthreads) - P.Gronek XII.2003
 *
 *  POTRZEBNE PLIKI
 *     mainW.c -- inicjalizacja i start obliczen.
 *     workW.c -- solwer numeryczny
 *     defsW.h -- plik naglowkowy
 *
 *  KOMPILACJA
 *
 *      cc -mt mainW.c workW.c  -lm
 *
 *  lub
 *      gcc mainW.c workW.c  -lm  -lthread
 *
 *  URUCHAMIANIE
 *
 *      ./a.out  liczba_watkow liczba_powtorzen
 *
 *       np.:
 *      ./a.out  5 6
 *
 *             gdzie:
 *
 *          liczba_watkow    - na ile rownych kawalkow ma byc
 *                            podzielony wektor niewiadomych
 *                            (tyle bedzie utworzonych watkow)
 *          liczba_powtorzen - tyle razy obliczenia beda
 *                             powtorzone zeby lepiej oszacowac czas
 *                             wykonania
 *
 *     Wywolanie bez parametrow:
 *           ./a.out
 *       jest rownowazne wywolaniu:
 *           ./a.out 2 1
 *
 *
 *    UWAGA !!!
 *   ----------
 *     Program wymaga dwoch plikow z danymi
 *             A.in oraz b.in
 *       A.in    -  plik zawiera najpierw wymiar zadania (liczbe n), a potem
 *                  po kolei elementy macierzy A wpisywane wierszami;
 *                  ma zatem n^2 +1 elementow
 *       b.in    -  plik zawiera najpierw wymiar zadania (liczbe n), a potem
 *                  po kolei elementy wektora b; ma zatem n+1 elementow
 *
 *
 *********************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
/* #include <asm/processor.h> - Solaris */

#include "defsW.h"


int     size;                    /* rozmiar zadania                 */
double  A[MAXSIZE][MAXSIZE];     /* macierz ukladu                  */
double  b[MAXSIZE];              /* wektor b                        */
double  xo[MAXSIZE];             /* wektor x                        */

int     *th_tab;                 /* Tablica numerow watkow          */
int     *local_stop;             /* Tablica znacznikow osiagniecia  */
                                 /* lokalnego testu stopu           */

int     p;                       /* liczba watkow                   */
int     num_processors;          /* liczba dostepnych procesorow    */
int     processor[64];

pthread_mutex_t read_write;
pthread_mutex_t stop_test;

int
main(int argc, char **argv)
{
        int i, it=1;
        double T;
        time_t start, stop;

        if (argc < 2)
                p = 2;
        else {
                p = atoi(argv[1]);
                if (argc == 3)
                        it = atoi(argv[2]);
                else
                        it = 1;
        }

        pthread_setconcurrency(p);

        getmem();
        readA();
        readb();
        start = time(NULL);
        for (i = 0; i< it; i++) {
                init();
        }
        stop = time(NULL);
        T = difftime(stop, start);
        printf("Time for %d iterations = %gs, Avr = %gs\n", it, T, T/it);
        freemem();
        return 0;
}

/*
 * Alokacja pamieci i inicjalizacja mechanizmow synchronizacji
 */
void
getmem()
{
        int i;

        /* Stwierdzenie liczby i numerow procesorow */
        num_processors = 0;
        for (i=0; i< 63;i++) {
              /*  if (p_online(i, P_STATUS) == P_ONLINE)  - Solaris */
                {
                        processor[num_processors++]=i;
                }
        }
#ifdef DEBUG
        printf("%d available processors\n", num_processors);
#endif

        local_stop = malloc(p*sizeof(int));
        th_tab = malloc(p*sizeof(int));

        /*      Nikt jeszcze nie skonczyl obliczen ;-) */
        memset(local_stop, 0, p*sizeof(int));
        memset(th_tab, 0, p*sizeof(int));

        pthread_mutex_init(&read_write, NULL);
        pthread_mutex_init(&stop_test, NULL);
}

/*
 * Zwolnienie pamieci dzielonej i semafora
 */
void
freemem()
{
        free(local_stop);
        free(th_tab);
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

        fscanf(afile,"%d", &size);

        for (i=0; i<size; i++) {
                for (j=0; j<size; j++) {
                        fscanf(afile,"%lf", &A[i][j]);
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

        fscanf(bfile,"%d", &size);

        for (i=0; i<size; i++) {
                fscanf(bfile,"%lf", &b[i]);
        }
        fclose(bfile);
}

/*
 * Podzial problemu i uruchomienie watkow obliczeniowych
 */
void
init()
{
        int i;
        int status;
        pthread_t t[50];

        /* Podzial zadania i uruchomienie watkow roboczych */
#ifdef DEBUG
        printf("size = %d\n", size);
#endif

        for (i=0; i<p; i++) {
                th_tab[i] = i;
                status = pthread_create(&t[i], NULL, work, (void*)&th_tab[i]);
                if (status) {
                        printf("problems... %d\n",status);
                        perror("");
                        exit (1);
                }
        }

        /* oczekiwanie na zakonczenie obliczen */
        for (i = 0; i<p; i++) {
                pthread_join(t[i], NULL);
#ifdef DEBUG
                printf("Finished %d\n", i);
#endif
        }

#ifdef RESULTS
        printf("size = %d ==>\n", size);
        for (i=0; i < size; i++) {
                printf("x%.2d ==> %f\n", i, xo[i]);
        }
#endif
        for (i = 0; i < p; i++) {
                if (local_stop[i]==0) {
                        printf("\a ERROR!!!\n");
                        exit(1);
                }
        }
}
