#include <stdlib.h>
#include <stdio.h>
#define ARSZ 20

int func(int x) {
  int array[x];
  array[x-1] = 10;
  printf("%d\n", array[x-1]);
  return 1;
}

int main() {

  func(10);
  
  int ar[ARSZ], i;
  for(i = 0; i < ARSZ; i++){
    ar[i] = i;
    i[ar]++;
    printf("ar[%d] now = %d\n", i, ar[i]);
  }

  printf("15[ar] = %d\n", 15[ar]);
  exit(EXIT_SUCCESS);


}
