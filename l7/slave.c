#include <stdio.h>
#include <pvm3.h>

int main()
{
  int myTID, masterTID;
  int x = 12;
  double begin, end;
  int num_points;

  myTID = pvm_mytid();
  printf("Slave1: TID is %d\n", myTID);

  pvm_recv(-1, -1);
  pvm_upkint(&x, 1, 1);
  printf("Slave1 has received x=%d\n", x);
  pvm_upkdouble(&begin, 1, 1);
  pvm_upkdouble(&end, 1, 1);
  pvm_upkint(&num_points, 1, 1);
  printf("TID %d : %f %f %d\n", myTID, begin, end, num_points);

  x = x + 5;

  pvm_initsend(PvmDataDefault);
  pvm_pkint(&x, 1, 1);

  pvm_send(pvm_parent(), 1);

  pvm_exit();

  return 0;
}
