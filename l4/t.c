#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#define BUFFOR_SIZE 80
#define VECTOR_KEY 0xDEADBEEE
#define RESULT_KEY 0x10BADBEE
#define RANGES_KEY 0x10DEEDEE
#define THREADS 5

/*ZMIENNE GLOBALNE*/
double	*_vector;
double 	_final_sum;
int *_ranges;
pthread_mutex_t _mutex;

double sum(double* vector, int a, int b) {
	double sum = 0.0;
   int i = 0;
   for (i=a; i<b; i++)
   	sum += vector[i];
   return sum;
}


void* sum_and_add(void* arg) {
	int *range_a = (int*) arg;
     
	double range_sum = sum(_vector, range_a[0], range_a[1]);

	pthread_mutex_lock(&_mutex);
	printf("Dodaję %g\n", range_sum);
   _final_sum += range_sum;
   pthread_mutex_unlock(&_mutex);
}

int read_vector_from_file(char *file_name) {
	int n = 0;
	FILE* f = fopen(file_name, "r");
	char buffer[BUFFOR_SIZE+1];
	
	/* pobranie długości wektora */
	fgets(buffer, BUFFOR_SIZE, f);
 	n = atoi(buffer);
	if(n == 0) {
		perror("Nie udało się pobrać liczby elementów wektora\n");
		exit(EXIT_FAILURE);
	}
	printf("Wektor ma %d elementów\n", n);

	/* alokowanie pamięci na wektor */
	prepare_vector(n); 

	/* wczytanie danych wektora */
   int i;
	for(i=0; i<n; i++) {
		fgets(buffer, BUFFOR_SIZE, f);
		_vector[i] = atof(buffer);
	}
	fclose(f); /* zamykamy plik z wektorem */

	return n;
}

int prepare_vector(int n) {
	size_t size = sizeof(double) * n;
	_vector = (double*) malloc(size);
   if (!_vector) {
        perror("malloc in vector");
        exit(EXIT_FAILURE);
   }
	return 0;
}

int prepare_ranges(int n){
	size_t size = sizeof(int) * (THREADS + 1);
	_ranges = (int*)malloc(size);
	if(!_ranges) {
		perror("malloc in ranges");
		return EXIT_FAILURE;
	}
	int i;
	int dn = n/THREADS; /* fragment wektora sumowany przez proces-dziecko */
	for(i=0; i<THREADS; i++) {
		_ranges[i] = i*dn;
	}
	_ranges[THREADS] = n;
	return 0;
}

void clean() {
	free(_vector);
	free(_ranges);
}

int main(int argc, char **argv) {
	int i, n;
	_mutex =  PTHREAD_MUTEX_INITIALIZER;
	_final_sum = 0;
	pthread_t threads[THREADS];

	n = read_vector_from_file("vector.dat");
	
	/* alokowanie pamięci na zakresy */
	prepare_ranges(n);

	printf("Suma początkowa: %g\n", _final_sum);
  	for (i = 0; i < THREADS; i++) {
   	if(pthread_create(&threads[i], NULL, &sum_and_add, &_ranges[i]) != 0) {
			clean();
         perror("Blad podczas tworzenia watku!\n");
         exit(EXIT_FAILURE);
      }
	}
	
	for (i = 0; i < THREADS; i++) {
   	if(pthread_join(threads[i], NULL) != 0) {
			clean();
         printf("Blad podczas czekania na watek %d!\n", i);
         exit(EXIT_FAILURE);
     	}
  	}

	clean();
	printf("Suma końcowa: %g\n", _final_sum);
	
	return 0;
}
