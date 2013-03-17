/*
Ćwiczenie PRIR - wątki
Artur Skonecki, Łukasz Załęski
Stworzenie indeksów określających podział danych obliczeniowych
https://github.com/skoneka/prir/blob/master/l4/matrix/main.c
--includes a cool thread pooling functionality--
*/

/* Co ten program robi:
 * Wczytanie macierzy z pliku oraz inicjalizacja macierzy wynikowej.
 *
 * Utworzenie tablicy na wątki o rozmiarze MAX_THREADS
 *
 * Pobranie wolnego wątku z puli za pomocą funkcji getFreeThread()
 *
 * Obliczenie cząstkowych wyników mnożenia przez poszczególne wątki.
 *
 * Obliczenie w poszczególnych wątkach sumy elementów fragmentów macierzy i zapisanie ich do zmiennej globalnej.
 *
 * Zebranie statusów zakończenia wątków.
 *
 * Wypisanie wyniku na ekran. (macierzy wynikowej oraz sumy elementów)
 *
 * Zwolnienie pamięci.
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#define MAX_THREADS 10

typedef struct matrix_s
{
    double **M;
    int rows;
    int colums;
} matrix_t;

typedef struct matrix_data_s
{
    matrix_t A;
    matrix_t B;
    matrix_t C;
} matrix_data_t;

typedef struct thread_data_s
{
    matrix_data_t *m;
    int i,j;
    int threadID;
} thread_data_t;

int checkFreeThread(int id);
void setThreadState(int id, char state);
char getThreadState(int id);
int getFreeThread();
void mulMatrix(matrix_data_t *m);
void makeMatrix(matrix_t *m);
void freeMatrix(matrix_t *m);
int filesToMatrix(matrix_data_t *m, char *fileNameA, char *fileNameB);
void printMatrix(matrix_t *m);
void sumCMatrixElements(matrix_data_t *m);

double theSum = 0;
pthread_t threads[MAX_THREADS]; /* array initialized to NULLs */
int activeThreads[MAX_THREADS]; /* array initialized to 1 */
/*
 * activeThreads[id] > 0 - zajety Thread
 * activeThreads[id] = 1 - nie uzyty thread lub zabezpieczony thread po pthread_join
 * activeThreads[id] = 2 - uzyty ale wolny thread
 */
pthread_mutex_t threadMutex = NULL;


int checkFreeThread(int id)
{
    pthread_mutex_lock(&threadMutex);
    if (activeThreads[id] > 0)
    {
        pthread_mutex_unlock(&threadMutex);
        return 1;
    }
    pthread_mutex_unlock(&threadMutex);
    return 0;
}

void setThreadState(int id, char state)
{
    pthread_mutex_lock(&threadMutex);
    /* VERBOSE fprintf(stderr, "setThreadState id=%d , state=%d\n", id, state); */
    activeThreads[id] = state;
    pthread_mutex_unlock(&threadMutex);
}

char getThreadState(int id)
{
    char state;
    pthread_mutex_lock(&threadMutex);
    state = activeThreads[id];
    pthread_mutex_unlock(&threadMutex);
    return state;
}

//int getUnusedThread

int getFreeThread()
{
    int i = 0;
    while(1) {
        if (i == MAX_THREADS)
            i = 0;

        /* VERBOSE fprintf(stderr, "checkFreeThread(%d)\n", i); */
        if(checkFreeThread(i)) {
        /* VERBOSE fprintf(stderr, "checkFreeThread(%d) = %d\n", i, i); */
            return i;
        }
        i++;
    }
}

void *mulThread(void *ptr)
{
    /* VERBOSE fprintf(stderr, "Thread number %ld\n", pthread_self()); */
    thread_data_t *threadData = (thread_data_t*)ptr;
    int thID = threadData->threadID;
    matrix_data_t *m = threadData->m;
    double s  = 0;
    int i = threadData->i, j = threadData->j;
    int k = 0;
    for(k; k < m->A.colums; k++)
    {
        s+= m->A.M[i][k] * m->B.M[k][j];
    }

    m->C.M[i][j] = s;

    free(threadData);
    setThreadState(thID, 2); // Free thread
    pthread_exit((void *)0);
}


void *sumThread(void *ptr)
{
    /* VERBOSE fprintf(stderr, "Thread number %ld\n", pthread_self()); */

    thread_data_t *threadData = (thread_data_t*)ptr;
    int thID = threadData->threadID;
    matrix_data_t *m = threadData->m;
    double s  = 0;
    int i = threadData->i;
    int k = 0;
    for(k; k < m->C.colums; k++)
    {
        s+= m->C.M[i][k];
    }
    
    pthread_mutex_lock(&threadMutex);
    theSum += s;
    pthread_mutex_unlock(&threadMutex);

    free(threadData);
    setThreadState(thID, 2); // Free thread
    
    pthread_exit((void *)0);
}

void sumCMatrixElements(matrix_data_t *m)
{
    int i, j;

    for ( i = 0; i < m->C.rows; i++ ) {
        int ret;
        int thID = getFreeThread();

        thread_data_t *threadData = malloc(sizeof(thread_data_t));
        threadData->i = i;
        threadData->threadID = thID;
        threadData->m = m;
        if(activeThreads[thID] == 2) //juz raz uzylismy...
        {
            /* VERBOSE fprintf(stderr, "Reusing thread... securing with pthread_join just in case\n"); */
            int *val;
            ret = pthread_join(threads[thID], (void **)&val);
            /* VERBOSE fprintf(stderr, "Thread %d exit status %d\n", thID, val);  */
            if( ret != 0) {
                /* VERBOSE fprintf(stderr, "%s:%d pthread_join fail %d\n", __FILE__, __LINE__, ret); */
                exit(EXIT_FAILURE);
            }
            else {
                /* VERBOSE fprintf(stderr, "%s:%d pthread_join OK\n", __FILE__, __LINE__); */
                activeThreads[thID] = 1;
            }
        }

        setThreadState(thID, 0);// Busy

        //raise(SIGINT);

        /* VERBOSE fprintf(stderr, "Thread %d\n", thID);  */
        ret = pthread_create( &threads[thID], NULL, sumThread, (void*) threadData); 
        if( ret != 0) {
            /* VERBOSE fprintf(stderr, "%s:%d pthread_create fail %d\n", __FILE__, __LINE__, ret); */
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0; i < MAX_THREADS;i++)
    {
        if(getThreadState(i) == 2 || getThreadState(i) == 0)
        {
            int *val;
            int ret = pthread_join(threads[i], (void **)&val);
            /* VERBOSE fprintf(stderr, "Thread %d exit status %d\n", i, val);  */
            if( ret != 0) {
                /* VERBOSE fprintf(stderr, "%s:%d pthread_join fail %d\n", __FILE__, __LINE__, ret); */
                exit(EXIT_FAILURE);
            }
            else {
                /* VERBOSE fprintf(stderr, "%s:%d pthread_join OK\n", __FILE__, __LINE__); */
                activeThreads[i] = 1;
            }
        }
    }
}


void mulMatrix(matrix_data_t *m)
{
    int i, j, k;

    for(i=0; i< m->A.rows; i++)
    {
        for(j=0; j< m->B.colums; j++)
        {
            int ret;
            int thID = getFreeThread();

            thread_data_t *threadData = malloc(sizeof(thread_data_t));
            threadData->i = i;
            threadData->j = j;
            threadData->threadID = thID;
            threadData->m = m;
            if(activeThreads[thID] == 2) //juz raz uzylismy...
            {
                /* VERBOSE fprintf(stderr, "Reusing thread... securing with pthread_join just in case\n"); */
                int *val;
                ret = pthread_join(threads[thID], (void **)&val);
                /* VERBOSE fprintf(stderr, "Thread %d exit status %d\n", thID, val);  */
                if( ret != 0) {
                    /* VERBOSE fprintf(stderr, "%s:%d pthread_join fail %d\n", __FILE__, __LINE__, ret); */
                    exit(EXIT_FAILURE);
                }
                else {
                    /* VERBOSE fprintf(stderr, "%s:%d pthread_join OK\n", __FILE__, __LINE__); */
                    activeThreads[thID] = 1;
                }
            }

            setThreadState(thID, 0);// Busy

            //raise(SIGINT);

            /* VERBOSE fprintf(stderr, "Thread %d\n", thID);  */
            ret = pthread_create( &threads[thID], NULL, mulThread, (void*) threadData); 
            if( ret != 0) {
                /* VERBOSE fprintf(stderr, "%s:%d pthread_create fail %d\n", __FILE__, __LINE__, ret); */
                exit(EXIT_FAILURE);
            }
        }
    }

    for (i = 0; i < MAX_THREADS;i++)
    {
        if(getThreadState(i) == 2 || getThreadState(i) == 0)
        {
            int *val;
            int ret = pthread_join(threads[i], (void **)&val);
            /* VERBOSE fprintf(stderr, "Thread %d exit status %d\n", i, val);  */
                if( ret != 0) {
                    /* VERBOSE fprintf(stderr, "%s:%d pthread_join fail %d\n", __FILE__, __LINE__, ret); */
                    exit(EXIT_FAILURE);
                }
                else {
                    /* VERBOSE fprintf(stderr, "%s:%d pthread_join OK\n", __FILE__, __LINE__); */
                    activeThreads[i] = 1;
                }
        }
    }
}

void makeMatrix(matrix_t *m)
{
    int i = 0;

    m->M = malloc(sizeof(double) * m->rows);
    for (i; i < m->rows;i++)
    {
        m->M[i] = malloc(sizeof(double) * m->colums);
        if(m->M[i] == NULL) {
            printf("failed to alloc mem");
        }
    }
}

void freeMatrix(matrix_t *m)
{
    int i = 0;
    for (i; i < m->rows;i++)
    {
        free(m->M[i]);
    }

    free(m->M);
}

int filesToMatrix(matrix_data_t *m, char *fileNameA, char *fileNameB)
{
    FILE *fA = fopen(fileNameA, "r");
    FILE *fB = fopen(fileNameB, "r");
    int i,j;

    if (!fA || !fB)
        return 1;

    fscanf(fA, "%d %d", &m->A.rows, &m->A.colums);
    fscanf(fB, "%d %d", &m->B.rows, &m->B.colums);

    makeMatrix(&m->A);
    makeMatrix(&m->B);
    m->C.rows = m->A.rows;
    m->C.colums = m->B.colums;
    makeMatrix(&m->C);

    for (i = 0;i < m->A.rows;i++)
    {
        for (j = 0; j < m->A.colums;j++)
        {
            fscanf(fA, "%lf", &(m->A.M[i][j]));
        }
    }

    for (i = 0;i < m->B.rows;i++)
    {
        for (j = 0; j < m->B.colums;j++)
        {
            fscanf(fB, "%lf", &(m->B.M[i][j]));
        }
    }
    fclose(fA);
    fclose(fB);

    return 0;
}

void printMatrix(matrix_t *m)
{
    int i,j;
    for (i = 0; i < m->rows;i++)
    {
        for (j = 0; j < m->colums;j++)
        {
            printf("%lf ", m->M[i][j]);
        }
        printf("\n");
    }
}

void init(void)
{
    int i;
    for ( i=0; i < MAX_THREADS; i++ ) activeThreads[i] = 1;
    for ( i=0; i < MAX_THREADS; i++ ) threads[i] = NULL;
}

int main(void)
{
    init();
    matrix_data_t m = {0};
    filesToMatrix(&m, "A.txt", "B.txt");

    /* A*B=C using thread pool */
    mulMatrix(&m);

    /* return codes of threads */

    printMatrix(&m.A);
    printf("---\n");
    printMatrix(&m.B);
    printf("---\n");
    printMatrix(&m.C);
    printf("---\n");

    /* mutex use show off */
    /* VERBOSE fprintf(stderr, "Sum C with threads\n"); */
    sumCMatrixElements(&m);
    printf("Sum = %f\n", theSum);

    freeMatrix(&m.A);
    freeMatrix(&m.B);
    freeMatrix(&m.C);
    return 0;
}

