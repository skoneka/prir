#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mpi.h"


#define send_data_tag 2001
#define send_child_data_tag 2002

#define ROOT 0

double 
f(double x)
{
	return x;
}

/*
 * a - start h - step index - shift
 */

//trapezoid
double 
integrate(double (*func)(double), double a, double h, int index)
{
	double		result = 0;
	double		ax = a + (h * (index-1));
	if (ax<0) ax =-ax;
	double		bx = a + (h * index);
	result = (bx - ax) * (func(ax) + func(bx)) / 2;
	if (result < 0) result = -result;
	//printf("integrating range a %f, h %f, index %d ; %f %f= %f\n", a,h, index,ax, bx,result);
	return result;
}


int 
main(int argc, char **argv)
{
	if (argc < 4) {
		printf("params req: a, b, n\n");
		return -1;
	}
	int		rank;
	int		size;
	int		n;
	int		i;
	double		w = 0; //total
	double		w_p = 0;// partial

	double		a;
	double		b;
	double		h;
	a = atof(argv[1]);
	b = atof(argv[2]);
	n = atoi(argv[3]);

	h = (b - a) / n;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
/*
	for (i = rank + 1; i <= n; i += size) {
		w_p += integrate(&f, a, h, i);

	}

*/


	if (rank == 0) {
		int els = n / size;
		int els_r= n % size;
		//printf("els %d els_r %d\n", els, els_r);
		int i;
		for( i = 1; i < size; i++) {
			int start = (i)*els;
			int end = start+els;
			//printf("s %d e %d\n", start, end);
			MPI_Send(&start, 1, MPI_INT, i, send_child_data_tag, MPI_COMM_WORLD);
			MPI_Send(&end, 1, MPI_INT, i, send_child_data_tag, MPI_COMM_WORLD);
		}
		//policz els_r

		



		MPI_Status	status;
		double		r;
		w += w_p;
		for (i = 1; i < size; i++) {
			MPI_Recv(&r, 1, MPI_DOUBLE, i, send_data_tag, MPI_COMM_WORLD, &status);
			w += r;
		}


		w+= integrate(&f,a,h,0);
		for (i = n-1; i>n-els; i--) {
			w+= integrate(&f,a,h,i);
		}

		printf("Result in range %lf do %lf = %lf\n", a, b, w);
	} else {
		int start;
		int end;
		int i;
		MPI_Status	status;
		MPI_Recv(&start, 1, MPI_INT, 0, send_child_data_tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&end, 1, MPI_INT, 0, send_child_data_tag, MPI_COMM_WORLD, &status);
		//printf("recv s %d e %d\n", start, end);
		for (i = start; i <= end; i++) {
			//printf("integrate %f %f %d\n", a, h, i);
			w_p += integrate(&f, a, h, i);
		}
		//printf("w_p result  %d do %d = %lf\n", start, end, w_p);
		MPI_Send(&w_p, 1, MPI_DOUBLE, ROOT, send_data_tag, MPI_COMM_WORLD);
	}
	MPI_Finalize();


	return 0;
}
