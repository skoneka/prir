#include <pthread.h>
pthread_mutex_t thMux;
pthread_t threads[1024];
char threadState[1024] = {0};

int getFreeThread(int N) {
    int i = 0;
    while(1) {
        if(i == N) i = 0;

        pthread_mutex_lock(&thMux);
        if(threadState[i] == 0) {
            pthread_mutex_unlock(&thMux);
            return i;
        }
        pthread_mutex_unlock(&thMux);
        i++;
    }
}

typedef struct ph_data  {
    double **A;
    double **B;
    double **C;
    int a,b;
    int c,d;
    int threadID;
    int k;
    int i, j;
} ph_data_t;

void mnozt(void *ptr) {
    ph_data_t *t = (ph_data_t)ptr;
    int s = 0;
    for(t->k=0; t->k<t->b; t->k++)
    {

        s+=t->A[t->i][t-k]*t->B[t->k][t->j];
    }
    t->C[t->i][t->j] = s;
}

void doJob(int tid, double **A, int a, int b, double **B, int c, int d, int k, int i, int j) {
    threadState[tid] = 1;
    ph_data_t *t = malloc(sizeof(ph_data_t));
    t->A = A;
    t->a = a;
    t->B = B;
    t->b = b;
    t->C = C;
    t->c = c;
    t->d = d;
    t->k = k;
    t->i = i;
    t->j = j;
    t->threadID = tid;

    pthread_create(&threads[tid], NULL, &mnozt, t);
}

void mnoz(double **A, int a, int b, double **B, int c, int d) {
    int i, j, k;
        double s;
        for(i=0; i<a; i++)
        {
            for(j=0; j<d; j++)
            {

                int thid = getFreeThread(N);
                    /*
                int s = 0;
                for(k=0; k<b; k++)
                {

                    s+=A[i][k]*B[k][j];
                }*/
                //C[i][j] = s;
            }

        }

}
