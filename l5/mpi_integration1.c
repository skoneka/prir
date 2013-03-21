#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mpi.h"


double f(double x){
    return x;
}

/* 
 * a - start
 * h - step
 * index - shift
 */
//simpson
double licz_czastke_s(double a, double h, int index)
{
	double result= 0;
	double ax= a + (h*(index-1));
	double bx= a + (h*index);
	result= ((bx-ax)/6)*(f(ax)+(4*f((ax+bx)/2)+f(bx)));
	return result;
}

//trapezoid
double licz_czastke_t(double a, double h, int index)
{
	double result= 0;
	double ax= a + (h*(index-1));
	double bx= a + (h*index);
	printf("ax = %f bx = %f\n", ax, bx);
	result= (bx-ax) * ( f(ax) + f(bx) ) / 2;
	return result;
}


int main(int argc, char** argv)
{
	double a;
	double b;
	double h;
	int rank;
	int size;
	int n;
	int i;
	double wynik= 0;
	double wynik_czastk= 0;

	if(argc<4){
		printf("params req: a, b, n\n");
		return -1;
	}
	
	a= atof(argv[1]);
	b= atof(argv[2]);
	n= atoi(argv[3]);
	
	h= (b-a)/n;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	for(i=rank+1; i<=n; i+=size){
		wynik_czastk+= licz_czastke_t(a, h, i);
		printf("rank %d\n", rank);
		
	}
	

	MPI_Reduce(&wynik_czastk, &wynik, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Finalize();

	if(rank==0) {
		printf("Result in range %lf do %lf = %lf\n", a, b, wynik);
		printf("rank %d pid %d\n", rank, getpid());
		//sleep(200);
		//raise(SIGINT);
	}


	return 0;
}
