/* -*- Mode: C; c-basic-offset:4 ; -*- */

#include <stdio.h>
#include "mpi.h"

#define send_data_tag 2001

int main( int argc, char *argv[] )
{
    int rank;
    int size;
    
    MPI_Init( 0, 0 );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    printf( "Hello world from process %d of %d\n", rank, size );

    if (rank == 0) {
        printf("rank %d\n", rank);
        int num = 25;
        MPI_Send ( &num, 1, MPI_INT, 1, send_data_tag, MPI_COMM_WORLD);
        printf("end rank %d\n", rank);
    }
    else {
        int r;
        int root_process = 0;
        MPI_Status status;
        MPI_Recv( &r, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
        printf("rank %d recv %d\n", rank, r);
    }


    MPI_Finalize();
    return 0;
}
