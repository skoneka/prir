#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

#define GROUP "GRUPA1234"

// liczba elementow przyp na proces
#define COUNT_JOBS 4

int main(int argc, char **argv)
{
	double got_vec[COUNT_JOBS];
	int ar = argc - 1;
	
	int len;
	//prowizoryczne wyznaczanie najblizszej wielokrotnosci COUNT_JOBS wiekszej niz ar
	if(ar % COUNT_JOBS != 0)
		len = ar + (COUNT_JOBS - (ar % COUNT_JOBS ));
	else
		len = ar;
	
	//printf("%d\n", len);
	int procNum = len/COUNT_JOBS - 1;
	printf("Child count: %d\n", procNum);

	int *tid;
	if(procNum)
		tid = malloc(sizeof(int) * procNum);

	double *vec = malloc(sizeof(double) * len);
	memset(vec, 0, sizeof(double) * len);

	int i = 1;
  	for(i; i < argc;i++)
  	{
  		vec[i-1] = atof(argv[i]);
		//printf("%lf\n", vec[i-1]);
  	}

  	int myTID = pvm_mytid();
  	printf("Master: TID is %d\n", myTID);
	
	pvm_joingroup(GROUP);
	// przemycamy ilosc dzieci do slavow
	if(procNum)
	{
  		pvm_spawn("slave", NULL, PvmTaskDefault, "", procNum, tid);
		for(i = 0;i < procNum;i++)
		{	
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&procNum, 1, 1);
			pvm_send(tid[i], 1);
		}
	}
	
	pvm_barrier(GROUP , procNum + 1);
	//printf("After barrier\n");

	pvm_scatter( got_vec, vec, COUNT_JOBS, PVM_DOUBLE, 0, GROUP, 0);

	double sum = 0;
	for(i = 0;i < COUNT_JOBS;i++)
	{
		//printf("Got:%lf\n", got_vec[i]);
		sum += got_vec[i];
	}
	
	pvm_reduce( PvmSum, &sum, 1, PVM_DOUBLE, 0, GROUP, 0);
	
	pvm_barrier(GROUP , procNum + 1);
	
	printf("Wynik %lf\n", sum);
	
	pvm_lvgroup(GROUP);
	
	if(procNum)
		free(tid);
	
	free(vec);
	pvm_exit();
}
