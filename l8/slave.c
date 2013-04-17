#include <stdio.h>
#include <pvm3.h>
#define GROUP "GRUPA1234"

// liczba elementow przyp na proces
#define COUNT_JOBS 4

int main(int argc, char **argv)
{
	int bufid = pvm_recv(-1, 1);
        if(bufid < 0)
        {
                perror("[SLAVE] Failed to get buff id");
                exit(0);
        }
	int procNum;
	pvm_upkint(&procNum, 1, 1);

	//printf("Proc num got: %d\n", procNum);
	pvm_joingroup(GROUP);
	pvm_barrier(GROUP , procNum + 1);
	
	double got_vec[COUNT_JOBS];
	pvm_scatter( got_vec, NULL, COUNT_JOBS, PVM_DOUBLE, 0, GROUP, 0);
	
	int i;
        double sum = 0;
        for(i = 0;i < COUNT_JOBS;i++)
        {
                //printf("Got:%lf\n", got_vec[i]);
                sum += got_vec[i];
        }
	printf("Local result: %lf\n", sum);
	
	pvm_reduce( PvmSum, &sum, 1, PVM_DOUBLE, 0, GROUP, 0);
        pvm_barrier(GROUP , procNum + 1);

	pvm_lvgroup(GROUP);
	pvm_exit();
}
