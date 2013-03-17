#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#define MAX_THREADS 3


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
int getFreeThread(int N);
void mulMatrix(matrix_data_t *m, int N);
void makeMatrix(matrix_t *m);
void freeMatrix(matrix_t *m);
int fileToMatrix(matrix_data_t *m, char *fileName);
void printMatrix(matrix_t *m);

pthread_t threads[MAX_THREADS] = {0};
char activeThreads[MAX_THREADS] = {1};
pthread_mutex_t threadMutex = {0};

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
    printf("setThreadState id=%d , state=%d\n", id, state);
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

int getFreeThread(int N)
{
    int i = 0;
    while(1) {
        if (i == N)
            i = 0;

        fprintf(stderr, "checkFreeThread(%d)\n", i);
        if(checkFreeThread(i)) {
        fprintf(stderr, "checkFreeThread(%d) = %d\n", i, i);
            return i;
        }
        i++;
    }
}

void *mulThread(void *ptr)
{
    printf("Thread number %ld\n", pthread_self());
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
}

void mulMatrix(matrix_data_t *m, int N)
{
    int i, j, k;

    for(i=0; i< m->A.rows; i++)
    {
        for(j=0; j< m->B.colums; j++)
        {
            int ret;
            int thID = getFreeThread(N);

            thread_data_t *threadData = malloc(sizeof(thread_data_t));
            threadData->i = i;
            threadData->j = j;
            threadData->threadID = thID;
            threadData->m = m;
            printf("Czekamy\n");
            if(activeThreads[thID] == 2) //juz raz uzylismy...
            {
                printf("Czekamy\n");
                ret = pthread_join(threads[thID], NULL);
                if( ret != 0) {
                    fprintf(stderr, "%s:%d pthread_join fail %d\n", __FILE__, __LINE__, ret);
                    exit(EXIT_FAILURE);
                }
                else
                    fprintf(stderr, "%s:%d pthread_join OK\n", __FILE__, __LINE__);
            }

            setThreadState(thID, 0);// Busy
            
            ret = pthread_create( &threads[thID], NULL, mulThread, (void*) threadData); 
            if( ret != 0) {
                fprintf(stderr, "%s:%d pthread_create fail %d\n", __FILE__, __LINE__, ret);
                exit(EXIT_FAILURE);
            }
        }
    }

    for (i = 0; i < N;i++)
    {
        if(getThreadState(i) == 2 || getThreadState(i) == 0)
        {
            pthread_join(threads[i], NULL);
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

int fileToMatrix(matrix_data_t *m, char *fileName)
{
    FILE *f = fopen(fileName, "r");
    int i,j;

    if (!f)
        return 1;

    fscanf(f, "%d %d", &m->A.rows, &m->A.colums);
    fscanf(f, "%d %d", &m->B.rows, &m->B.colums);

    makeMatrix(&m->A);
    makeMatrix(&m->B);
    m->C.rows = m->A.rows;
    m->C.colums = m->B.colums;
    makeMatrix(&m->C);

    for (i = 0;i < m->A.rows;i++)
    {
        for (j = 0; j < m->A.colums;j++)
        {
            fscanf(f, "%lf", &(m->A.M[i][j]));
        }
    }

    for (i = 0;i < m->B.rows;i++)
    {
        for (j = 0; j < m->B.colums;j++)
        {
            fscanf(f, "%lf", &(m->B.M[i][j]));
        }
    }
    fclose(f);

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

int main(void)
{
    matrix_data_t m = {0};
    fileToMatrix(&m, "matrix.txt");
    mulMatrix(&m, 4);
    printMatrix(&m.A);
    printf("---\n");
    printMatrix(&m.B);
    printf("---\n");
    printMatrix(&m.C);

    freeMatrix(&m.A);
    freeMatrix(&m.B);
    freeMatrix(&m.C);
    return 0;
}

