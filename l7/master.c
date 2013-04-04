/*
 * lab PRiR pvm 1
 * Zadanie polega na zaimplementowaniu programu liczącego całkę jednej zmiennej przy pomocy metody prostokątów/trapezów/simpsona (tak jak w zadaniu z Ćwiczenia 3), z wykorzystaniem PVM.
 * 2013 Artur Skonecki, Łukasz Kalinowski
 * $ pvm
 * pvm> spawn -> master 0 5 1000
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <math.h>

#define NPROC 3

int main(int argc, char **argv)
{
  int myTID;

  int tids[NPROC] = {[0 ... NPROC-1] = 0};
  int res;

  myTID = pvm_mytid();
  printf("Master: TID is %d\n", myTID);

  double          a;
  double          b;
  int          num_points;
  a = atof(argv[1]);
  b = atof(argv[2]);
  num_points = atoi(argv[3]);

  double begins[NPROC];
  double ends[NPROC];

  double basic_range = (b-a)/NPROC;
  //double extra_range = fmod((b-a),NPROC);

  for (int procindex = 0; procindex < NPROC; procindex++) {
    begins[procindex] = procindex * basic_range;
    ends[procindex] = begins[procindex] + basic_range;

    printf("begins[%d]= %f ends[%d]= %f\n", procindex,begins[procindex], procindex,ends[procindex]);
  }

  printf ("basic_range = %f\n", basic_range);
  

  res = pvm_spawn("slave", NULL, PvmTaskDefault, "", NPROC, tids);
  printf("res %d\n", res);
  if (res<1) {
    printf("Master: pvm_spawn error\n");
    pvm_exit();
    exit(1);
  }
  //for (int procindex = 0; procindex < NPROC; procindex++) printf("tids[%d] = %d\n", procindex, tids[procindex]);

  for (int procindex = 0; procindex < NPROC; procindex++) {
    pvm_initsend(PvmDataDefault);
    pvm_pkdouble(&begins[procindex], 1, 1);
    pvm_pkdouble(&ends[procindex], 1, 1);
    pvm_pkint(&num_points, 1, 1);

    pvm_send(tids[procindex], 1);
  }

  double result=0;
  for (int procindex = 0; procindex < NPROC; procindex++) {
    double r=0;
    pvm_recv(-1, -1);
    pvm_upkdouble(&r, 1, 1);

    printf("Master has received r=%f\n", r);
    result += r;
  }
  printf("Result %f\n", result);

  pvm_exit();

  return 0;
}
