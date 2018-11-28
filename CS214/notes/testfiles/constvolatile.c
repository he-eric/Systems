#include <stdio.h>
#include <stdlib.h>

extern volatile int clock;

int main (int argc, char **argv) {
  
  int const i = 5;
  printf("const i: %d\n", i);
  
  //clock = 0;

  //bus error 10
  /*
  while (clock < 10) {
    fprintf(stdout, "%d", clock);
    clock++;
  }
  */
  
  //fprintf(stdout, "%p", &clock);
  
  const int local = 10;
  int *ptr = (int*) &local;
  
  printf("\nconst int local = 10\n");
  printf("int *ptr = (int*) &local\n");
  printf("\nMemory location of const int local: %p\n", &local);
  printf("Memory location of int *ptr: %p\n", ptr);

  printf("\nChanging the value of ptr will not change the value of local even though they point to the same memory location\n");

  printf("\nInitial value of local : %d \n", local);
 
  *ptr = 100;
 
  printf("Modified value of local: %d \n", local);
  printf("Value of ptr: %d \n", *ptr);

  const int p[3] = {1, 2, 3};

  printf("\nconst int p[3] = {1, 2, 3}\n"); //pointer to const int
  printf("\nconst int p[0]: %d\n", p[0]);
  printf("const int p[1]: %d\n", p[1]);
  printf("const int p[2]: %d\n", p[2]);
  printf("\nChanging the values of p[i] is not possible\n");
  
  int ip[3] = {1, 2, 3}; //const pointer to non-const int
  
  printf("\nint ip[3] = {1, 2, 3}\n");
  printf("\nint ip[0]: %d\n", ip[0]);
  printf("int ip[1]: %d\n", ip[1]);
  printf("int ip[2]: %d\n", ip[2]);
  
  ip[0] = 3;
  ip[1] = 2;
  ip[2] = 1;
  
  printf("\nDereferences ip and then increments value at pointer ip[0]\n");
  printf("ip[0]++: %d\n", ip[0]++); 
  printf("\nIncrements pointer ip but ip is a const int *\n");
  printf("ip++: Error\n");

  printf("\nModified values on int ip[3]\n");
  printf("int ip[0]: %d\n", ip[0]);
  printf("int ip[1]: %d\n", ip[1]);
  printf("int ip[2]: %d\n", ip[2]);

  int const * w = (int *)(ip);
  
  printf("\nint const * w = ip\n");
  printf("int const w[0]: %d\n", w[0]);
  printf("int const w[1]: %d\n", w[1]);
  printf("int const w[2]: %d\n", w[2]);
  
  //int x = 1;
  //int * const q = &x;
  //q[0]++;
  

  /* w is a read-only variable error
  printf("int const w[0]: %d\n", w[0]);
  printf("int const w[0]++: %d\n", w[0]++);
  printf("int const w[1]++: %d\n", w[1]++);
  */
  
  const int * const ccp; //const int pointer that holds a const int
  const char * c = "776"; //const char pointer

  return EXIT_SUCCESS;
}
