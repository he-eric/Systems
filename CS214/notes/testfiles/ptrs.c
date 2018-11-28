#include <stdlib.h>
#include <stdio.h>

int minusone(int i) {
  return i-1;
}


int main() {
  
  char memory[5000];

  printf("memory:     %p\n\n", memory);
  printf("memory+16:  %p\n\n", memory+16);
  printf("&memory[16] %p\n\n", &memory[16]);
  
  char *c = memory;
  int *i = (int *)memory; //interpret first 4 bytes of memory as int
  *i = 5;

  printf("*(int *)memory: %d\n\n", *(int *)memory);

  printf("i+1:       %p\n\n", i+1);
  printf("&memory[4] %p\n\n", &memory[4]);

  *(i+1) = 6;

  printf("*(int *)memory: %d\n\n", *(int *)(memory+4));

  char foo[6] = "hello\0";
  foo[0] = 'c';
  printf("foo: %s\n\n", foo);

  const char * ptr = "hello";
  //ptr[0] = 'd';
  
  printf("ptr: %s\n\n", ptr);

  if ('\0') {
    printf("\\0 is null\n");
  }

  printf("sizeof(ptr): %lu\n\n", sizeof(ptr));
  printf("*foo:   %c\n", *foo);
  printf("foo[0]: %c\n", foo[0]);

  int (*funptr)(int) = minusone;
  printf("\n(*funptr)(3): %d\n", (*funptr)(3));
  printf("funptr(3):      %d\n", funptr(3));
  printf("sizeof(funptr): %lu\n", sizeof(funptr));
  

  return 0;
  
}
