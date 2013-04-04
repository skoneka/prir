#include <stdio.h>
#include <pvm3.h>

double f(double x)
{
  return x*x;
}


//trapezoid
double
trapezoid(double begin, double h, int index)
{
  double          result = 0;
  double          ax = begin + (h * (index - 1));
  double          bx = begin + (h * index);
  result = (bx - ax) * (f(ax) + f(bx)) / 2;
  return result;
}

int main()
{
  int myTID, masterTID;
  double result = 0;
  double begin, end;
  int num_points;

  myTID = pvm_mytid();
  printf("Slave1: TID is %d\n", myTID);

  pvm_recv(-1, -1);
  pvm_upkdouble(&begin, 1, 1);
  pvm_upkdouble(&end, 1, 1);
  pvm_upkint(&num_points, 1, 1);
  printf("TID %d : %f %f %d\n", myTID, begin, end, num_points);

  double h = ( end - begin ) / num_points;

  for (int i = 1; i < num_points; i++) {
    result += trapezoid(begin, h, i);
  }
  
  pvm_initsend(PvmDataDefault);
  pvm_pkdouble(&result, 1, 1);
  pvm_send(pvm_parent(), 1);

  pvm_exit();

  return 0;
}
