#include <stdio.h>
#include <pvm3.h>

int main()
{
  int mytid;

  mytid = pvm_mytid();
  printf("My TID is %d\n", mytid);
  pvm_exit();
  return 0;
}
