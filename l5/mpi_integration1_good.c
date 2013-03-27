#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mpi.h"


#define send_data_tag 2001

#define ROOT 0

double 
f(double x)
{
	return x;
}

/*
 * a - start h - step index - shift
 */
//simpson
double 
simpson(double a, double h, int index)
{
	double		result = 0;
	double		ax = a + (h * (index - 1));
	double		bx = a + (h * index);
	result = ((bx - ax) / 6) * (f(ax) + (4 * f((ax + bx) / 2) + f(bx)));
	return result;
}

//trapezoid
double 
trapezoid(double a, double h, int index)
{
	double		result = 0;
	double		ax = a + (h * (index - 1));
	double		bx = a + (h * index);
	result = (bx - ax) * (f(ax) + f(bx)) / 2;
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

	for (i = rank + 1; i <= n; i += size) {
		w_p += trapezoid(a, h, i);

	}

	if (rank == 0) {
		MPI_Status	status;
		int		i;
		double		r;
		w += w_p;
		for (i = 1; i < size; i++) {
			MPI_Recv(&r, 1, MPI_DOUBLE, i, send_data_tag, MPI_COMM_WORLD, &status);
			w += r;
		}

		printf("Result in range %lf do %lf = %lf\n", a, b, w);
	} else {
		MPI_Send(&w_p, 1, MPI_DOUBLE, ROOT, send_data_tag, MPI_COMM_WORLD);
	}
	MPI_Finalize();


	return 0;
}
