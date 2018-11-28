#include <stdlib.h>
#include <stdio.h>
#define ARLEN 10

int
main() {
  int ar[ARLEN], *ip;
  
  ip = ar;
  printf("ip: %p\n", ip);
  while(ip < &ar[ARLEN]) {
    *ip = 0;
    
    printf("ip: %p %d\n", ip, *ip);
    ip++;
  }

}
