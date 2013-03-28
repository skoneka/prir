#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "math.h"
#include "mpi.h"

int 
main(int argc, char **argv)
{
	int		taskid    , ntasks;
	MPI_Status	status;
	int		ierr      , i, j, itask;
	int		buffsize;
	double         *sendbuff, *recvbuff, buffsum, totalsum;
	double		inittime, totaltime;

	/* =============================================================== */
	/* MPI Initialisation. It's important to put this call at the    */
	/* begining of the program, after variable declarations.         */
	MPI_Init(&argc, &argv);

	/* =============================================================== */
	/* Get the number of MPI tasks and the taskid of this task.      */
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	/* =============================================================== */
	/* Get buffsize value from program arguments.                    */
	buffsize = atoi(argv[1]);

	/* =============================================================== */
	/* Printing out the description of the example.                  */
	if (taskid == 0) {
		printf("\n\n\n");
		printf("##########################################################\n\n");
		printf(" Example 12 \n\n");
		printf(" Collective Communication : MPI_Reduce \n\n");
		printf(" Vector size: %d\n", buffsize);
		printf(" Number of tasks: %d\n\n", ntasks);
		printf("##########################################################\n\n");
		printf("                --> BEFORE COMMUNICATION <--\n\n");
	}
	/* ============================================================= */
	/* Memory allocation.                                          */
	sendbuff = (double *)malloc(sizeof(double) * buffsize);
	recvbuff = (double *)malloc(sizeof(double) * buffsize);

	/* ============================================================= */
	/* Vectors and/or matrices initalisation.                      */
	srand((unsigned)time(NULL) + taskid);
	for (i = 0; i < buffsize; i++) {
		sendbuff[i] = (double)rand() / RAND_MAX;
	}

	/* ============================================================== */
	/* Print out before communication.                              */

	MPI_Barrier(MPI_COMM_WORLD);

	buffsum = 0.0;
	for (i = 0; i < buffsize; i++) {
		buffsum = buffsum + sendbuff[i];
	}
	printf(" Task %d : Sum of sendbuff elements = %e \n", taskid, buffsum);

	MPI_Barrier(MPI_COMM_WORLD);

	ierr = MPI_Reduce(&buffsum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (taskid == 0) {
		printf("                                     =============\n");
		printf("                              TOTAL: %e \n\n", totalsum);
	}
	/* =============================================================== */
	/* Communication.                                                */

	inittime = MPI_Wtime();

	ierr = MPI_Reduce(sendbuff, recvbuff, buffsize, MPI_DOUBLE, MPI_SUM,
			  0, MPI_COMM_WORLD);

	totaltime = MPI_Wtime() - inittime;

	/* =============================================================== */
	/* Print out after communication.                                */
	if (taskid == 0) {
		printf("##########################################################\n\n");
		printf("                --> AFTER COMMUNICATION <-- \n\n");
		buffsum = 0.0;
		for (i = 0; i < buffsize; i++) {
			buffsum = buffsum + recvbuff[i];
		}
		printf(" Task %d : Sum of recvbuff elements -> %e \n", taskid, buffsum);
		printf("\n");
		printf("##########################################################\n\n");
		printf(" Communication time : %f seconds\n\n", totaltime);
		printf("##########################################################\n\n");
	}
	/* =============================================================== */
	/* Free the allocated memory.                                    */
	free(recvbuff);
	free(sendbuff);

	/* =============================================================== */
	/* MPI finalisation.                                             */
	MPI_Finalize();

}
