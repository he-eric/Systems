#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void convert( char *str) {
  int i;
  
  for (i=0; i<=strlen(str); i+=4) {
    printf("%d", *(int *)(&str[i]));
  }
  printf("1\n");
}

int main() {

  typedef struct node {
    int i;
    struct node * next;
  } node;

  typedef union {
    float f;
    int i;
    char str[20];
  } Data; 

  Data n;
  
  n.f = 220.5;
  n.i = 10;
  strcpy( n.str, "Hello" );
 
  printf("n.float: %f\nn.int: %d\nn.string: %s\nsizeof(n): %lu\n", n.f, n.i, n.str, sizeof(n));
  
  /* 
  char * l = malloc(sizeof(char)*10);
  l = "foo\0";
  printf("%s\n", l);
  */

  int * i = (int *)malloc(4*sizeof(int));
  int size = 0;
  size = (i+1)-i;
  printf("size: %d\n", size);

  char c[10] = "jack\0";
  printf("\nstrlen(c) = %lu\n", strlen(c));
  printf("sizeof(c) = %lu\n\n", sizeof(c));
  
  /*
  char * jack = malloc(sizeof(char)*4);
  jack = "jack";
  printf("jack: %s\n", jack);
  convert(jack);
  */

  node * front = malloc(sizeof(struct node));
  front->i = 5;
  front->next = NULL;
  printf("front->i:    %d\n", front->i);
  printf("front->next: %p\n", front->next);

  typedef enum Week {MON=1, TUES=2, WED=3, THURS=4, FRI=5} Weekday;
  Weekday day = MON;
  printf("\nsizeof(day): %lu\n", sizeof(day));
  printf("sizeof(MON): %lu\n", sizeof(MON));
  printf("MON=%d\n", MON);

  return EXIT_SUCCESS;

}
