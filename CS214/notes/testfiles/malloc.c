#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


int main (int argc, char ** argv) {
  
  char * ptr = malloc(sizeof(char)*20);
  ptr = strcpy(ptr, "hello");
 
  printf("ptr: %s\n", ptr);
  
  ptr = realloc(ptr, sizeof(char)*1);
  
  printf("strlen(ptr): %lu\n", strlen(ptr));
  printf("ptr: %s\n", ptr);

  free(ptr);

  ptr[0] = 'm';

  printf("ptr: %s\n", ptr);
  printf("ptr+1: %s\n", ptr+1);

  return EXIT_SUCCESS;
  
}
