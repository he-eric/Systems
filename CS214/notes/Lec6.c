#include <stdlib.h>
#include <stdio.h>

#define SQUARE(x) ((x)*(x))
#define CUBE(x) x*SQUARE(x)

//Note to self Calvin Tang took 9% battery from laptop

int main(int argc, char ** argv) {
  int i = 1;
  //Strings
  
  char c = '\'';
  char c2 = '"';
  printf("%c %c\n", c, c2);
  
  char *str = "He said\"hello\".";
  printf("%s\n", *str);
  /*
    \n
    \t
    \r - return
    \b - backspace
  */
  printf("x\b`\n");

  /*
    %d
    %u 
    %% - print % using printf
  */
  char *foo = "I am 50% ok";
  printf("%%\n"); printf("%s\n", *foo);

  //"a" "b" is the same as "ab"
  char *longstring = "this string is long"
    "I wish I could put it on multiple lines";
  printf("%s\n", *longstring);
  
  /*switch
    switch(expression) {
    case 1:
    break;
    case 2:
    break;
    case 3:
    
    default:
    
    }
    duff's device
  */

  //macros
#define MEMORY_SIZE 1
  static char[MEMORY_SIZE];
  while (MEMORY_SIZE > i) {
  }
  printf("MEMORY_SIZE won't get replaced\n");
 
  /* Doesnt work
     #define EXISTS !=0
     if (p EXISTS) {
     }
  */
  
  int y = 3;
  printf("%d\n", CUBE(y));
#undef SQUARE
#define SQUARE(x) ((x)+1)
  printf("%d\n", CUBE(y));
	 
  /*
    #define SQUARE(x) x*x
    //isalpha() is a macro <ctype.h>
    int var = 3;
    SQUARE(var);
    SQUARE(var+3);

    //better
    #define SQUARE(x) (x)*(x)
    SQUARE(var+3) => (var+3)*(var+3)

    //even better
    #define SQUARE(x) ((x)*(x))
    
    //weird side effects?
    SQUARE(x++) => ((x++)*(x++))

    Macros can return anything
    #define ASSERT(p, msg) if (!p) { fprintf(stderr, "%s\n", msg) }; exit(0);
    ASSERT(p != NULL, "p is NULL");

    You can include macros in your macro definitions
    #define CUBE(x) x*SQUARE(x)

    #define RECURSIVE 1+RECURSIVE
    Undefined since RECURSIVE is not already defined...so no recursion
  */

  //fun with pointers
  int x = 1;
  x = x + 1; 
  //left x location x in memory or register
  //right x value x
  //&x = y; not allowed; taking address from stack and pointing elsewhere
  //&x can happen on the right side of assignment
  //y = &x
  
  //register int r;
  //&r; not allowed
  
  int *p;
  p = &x;
  //y = *p; store p value at y
  //*p = y; store y at p
  int array[100];
  array[0] = 4;
  

  //pointers to functions
  /*
    int minusone(int x) {
     return x-1;
    } 8====D-----------Ca
    int (*func)(int) = minusone
    int two = funptr(3);
  */
  return 0;
}
