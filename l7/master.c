#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <math.h>

#define NPROC 2
#define NTIDS 4

int main(int argc, char **argv)
{
  int myTID;
  int x = 12;
  int tids[NTIDS] = {[0 ... NTIDS-1] = 0};
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
  
//   res = pvm_spawn("slave", NULL, PvmTaskDefault, "", 1, tids);
//   printf("res %d\n", res);
//   if (res<1) {
//     printf("Master: pvm_spawn error\n");
//     pvm_exit();
//     exit(1);
//   }

  for (int procindex = 0; procindex < NTIDS; procindex++) printf("tids[%d] = %d\n", procindex, tids[procindex]);
  
  res = pvm_spawn("slave", NULL, PvmTaskDefault, "", 2, tids);
  printf("res %d\n", res);
  if (res<1) {
    printf("Master: pvm_spawn error\n");
    pvm_exit();
    exit(1);
  }
  for (int procindex = 0; procindex < NTIDS; procindex++) printf("tids[%d] = %d\n", procindex, tids[procindex]);

  for (int procindex = 0; procindex < NPROC; procindex++) {
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&x, 1, 1);
    pvm_pkdouble(&begins[procindex], 1, 1);
    pvm_pkdouble(&ends[procindex], 1, 1);
    pvm_pkint(&num_points, 1, 1);

    pvm_send(tids[procindex], 1);
/*
    pvm_recv(-1, -1);
    pvm_upkint(&x, 1, 1);

    printf("Master from tids[%d] has received x=%d\n", tids[procindex], x);*/
  }
  for (int procindex = 0; procindex < NPROC; procindex++) {
    pvm_recv(-1, -1);
    pvm_upkint(&x, 1, 1);

    printf("Master has received x=%d\n", x);
  }

  pvm_exit();

  return 0;
}
