#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define MAX_MALLOC 150

void caseA(){
  //A: malloc() 1 byte and immediately free it - do this 150 times
  int i;
  for (i = 0; i < MAX_MALLOC; i++) {
    char * p = malloc(1);
    free(p);
  }
}

void caseB(){
  /*
    B: malloc() 1 byte, store the pointer in an array - do this 150 times.
    Once you've malloc()ed 150 byte chunks, then free() the 150 1 byte pointers one by one.
  */
  int i;
  char *temp[MAX_MALLOC];
  for (i = 0; i < MAX_MALLOC; i++) {
    temp[i] = malloc(1);
  }
  for (i = 0; i < MAX_MALLOC; i++) {
    free(temp[i]);
  }
}

void caseC(){
  /*
    C: Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer - do this 150 times
    - Keep track of each operation so that you eventually malloc() 150 bytes, in total
    - Keep track of each operation so that you eventually free() all pointers
    (don't allow a free() if you have no pointers to free())
  */
  char *temp2[MAX_MALLOC];
  int malloc_count = 0;
  int free_count = 0;
  int allocated = 0;
  srand(time(0));

  while ( (malloc_count <= MAX_MALLOC) && (free_count <= MAX_MALLOC) ) {

    //0 - Free
    //1 - Malloc
    int op = rand() % 2;
    
    //If reached MAX_MALLOC's, then free remaining allocated pointers 
    if(malloc_count == 150){
      while(allocated >0){
	free(temp2[allocated-1]);
	free_count++;
	allocated--;
      }
      break;
    }

    //If havent reached MAX_MALLOC's, and 0 allocated
    //Must malloc
    else if(allocated == 0){
      temp2[allocated] = malloc(1);
      malloc_count++;
      allocated++;
    } 

    //If havent reached MAX_MALLOC's, at least 1 allocated, and op returns 1
    //Malloc
    else if (op){
      temp2[allocated] = malloc(1);
      malloc_count++;
      allocated++;
    } 
    
    //If havent reached MAX_MALLOC's, at least 1 allocated, and op returns 0
    //Free
    else {
      free(temp2[allocated-1]);
      free_count++;
      allocated--;
    }
  }
}

void caseD(){
  /*
    D: Randomly choose between a randomly-sized malloc() or free()ing a pointer – do this many times 
    - Keep track of each malloc so that all mallocs do not exceed your total memory capacity
    - Keep track of each operation so that you eventually malloc() 150 times
    - Keep track of each operation so that you eventually free() all pointers
    - Choose a random allocation size between 1 and 64 bytes
  */

  char *temp2[MAX_MALLOC];
  int malloc_count = 0;
  int allocated = 0;
  int i;
  
  //Set inital array of pointers to NULL
  //NULL will signify freed pointers
  for(i = 0;i<MAX_MALLOC;i++){
    temp2[i] = NULL;
  }
  
  srand(time(0));
  while ( (malloc_count < MAX_MALLOC) ) {
    
    int op = rand() % 2;
    int malloc_size = (rand() % 64) + 1;
    
    //If havent reached MAX_MALLOC's, and 0 allocations
    //Must malloc
    if(allocated == 0){
      temp2[allocated] = malloc(malloc_size);
      malloc_count++;
      allocated++;
    }
    
    //If havent reached MAX_MALLOC's, at least 1 allocation, and op returns 1
    else if (op){
      temp2[allocated] = malloc(malloc_size);
      
      //If temp2[allocated] returns NULL, malloc failed; out of memory capacity
      //Must free previous index and set index to NULL to signify freed pointer
      if(temp2[allocated] == NULL){
	free(temp2[allocated-1]);
	temp2[allocated-1] = NULL;
	allocated--;
      } 
      
      //If temp2[allocated] returns not NULL, malloc succeded
      //Increment neccessary counters
      else {
	malloc_count++;
	allocated++;
      }	
    } 

    //If havent reached MAX_MALLOC's, at least 1 allocation, and op returns 0
    //Free pointer at index and set to NULL to signify freed pointer
    else {
      free(temp2[allocated-1]);
      temp2[allocated-1] = NULL;
      allocated--;
    }
  }
  
  //If reached MAX_MALLOC's, free remaining pointers
  while(allocated > 0){
    free(temp2[allocated-1]);
    temp2[allocated-1] = NULL;
    allocated--;
  }

  //Test if all pointers were really freed
  /*
  for(i = 0;i<150;i++){
    if(temp2[i] != NULL){
      printf("%d not null\n", i);
    }
  }
  */
}

void caseE(){
  /*E: Fill an array of 150 pointers of size one byte
    - Free 75 randomly chosen indexes of the array
    - Malloc 75 randomly chosen indexes to test the coalesce function
    - If the index chosen is already allocated, do nothing
    - Keep track of each malloc so that all mallocs do not exceed your total memory capacity
    - Keep track of each operation so that you eventually free() all pointers
  */
  
  char* temp[MAX_MALLOC];
  int i;
  int free_count = 0;
  
  //MALLOC ALL INDEXES OF ARRAY WITH 1 BYTE
  for(i = 0;i < MAX_MALLOC;i++){
    temp[i] = malloc(1);
  }

  srand(time(0));
  i = 0;
  
  //FREE 75 RANDOM POINTERS
  while(free_count < 75){
    
    //If reached end of array without freeing 75, reset index to 0
    if(i == MAX_MALLOC){
      i = 0;
    }

    //If current index is NULL move to next index
    if(temp[i] == NULL){
      i++;
      continue;
    }
    
    //If current index is not NULL, flip a coin to choose between freeing or doing nothing
    int op = rand() % 2;
    if(op){
      free(temp[i]);
      temp[i] = NULL;
      free_count++;
    } 
    i++;
  }
  
  //CHOOSE 75 RANDOM INDEXES
  //ATTEMPT TO MALLOC 1 BYTE
  //IF ALREADY MALLOCED, DO NOTHING
  for(i = 0;i < 75;i++){
    
    int rand_index = rand() % 150;
    if(temp[rand_index] == NULL){
      temp[rand_index] = malloc(1);
    }
    
  }

  //FREE ALL POINTERS
  for(i = 0;i < MAX_MALLOC;i++){
    if(temp[i] != NULL){
      free(temp[i]);
    }
  }

}

void caseF(){
  /*F: Fill an array of 150 pointers of random size between 1 and 64
    - Free 75 randomly chosen indexes of the array
    - Malloc 75 randomly chosen indexes to test the coalesce function
    - If the index chosen is already allocated, do nothing
    - Keep track of each operation so that you eventually free() all pointers
  */

  char* temp[150];
  int i = 0;
  int malloc_size;
  
  srand(time(0));
  
  //MALLOC ALL INDEXES OF ARRAY WITH RANDOM SIZE
  while(i < 150){
    malloc_size = (rand() % 64) + 1;
    temp[i] = malloc(malloc_size);
    i++;
  }
  
  int free_count = 0;
  i = 0;
  
  //FREE 75 RANDOM POINTERS
  while(free_count < 75){
    
    //If reached end of array without freeing 75, reset index to 0 
    if(i == 150){
      i = 0;
    }

    //If current index is NULL move to next index
    if(temp[i] == NULL){
      i++;
      continue;
    }

    //If current index is not NULL, flip a coin to choose between freeing or doing nothing
    int op = rand() % 2;
    if(op){
      free(temp[i]);
      temp[i] = NULL;
      free_count++;
    } 
    i++;
  }

  //CHOOSE 75 RANDOM INDEXES
  //ATTEMPT TO MALLOC A RANDOM SIZE
  //IF ALREADY MALLOCED, DO NOTHING
  for(i = 0;i < 75;i++){
    malloc_size = (rand() % 64) + 1;
    int rand_index = rand() % 150;
    if(temp[rand_index] == NULL){
      temp[rand_index] = malloc(malloc_size);
    }
  }
  
  //FREE ALL POINTERS
  for(i = 0;i < 150;i++){
    if(temp[i] != NULL){
      free(temp[i]);
    }
  }
}

/*
  Function calculate_time:
  Calculates in seconds the difference between start time and end time
 */
double calculate_time(struct timeval start,struct timeval end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0;
}

int main(int argc, char **argv) {

  struct timeval start, end;
  initmem();
  
  int i;
  double sum[6] = {0,0,0,0,0,0};
  
  for(i = 0;i<100;i++){ 
    //At this point our heap has been separated into many free blocks. Call initmem to immediately
    //coalesce
    initmem();
    gettimeofday(&start,NULL);
    caseA();
    gettimeofday(&end,NULL);
    sum[0] += calculate_time(start,end);
    gettimeofday(&start,NULL);
    caseB();
    gettimeofday(&end,NULL);
    sum[1] += calculate_time(start,end);
    gettimeofday(&start,NULL);
    caseC();	
    gettimeofday(&end,NULL);
    sum[2] += calculate_time(start,end);
    gettimeofday(&start,NULL);
    caseD();
    gettimeofday(&end,NULL);
    sum[3] += calculate_time(start,end);
    gettimeofday(&start,NULL);
    caseE();
    gettimeofday(&end,NULL);
    sum[4] += calculate_time(start,end);
    gettimeofday(&start,NULL);
    caseF();
    gettimeofday(&end,NULL);
    sum[5] += calculate_time(start,end);
  }
  
  for(i = 0;i<6;i++){
    switch(i){
    case 0:
      printf("Case A: %f\n", sum[0]/100);
      break;
    case 1:
      printf("Case B: %f\n", sum[1]/100);
      break;
    case 2:
      printf("Case C: %f\n", sum[2]/100);
      break;
    case 3:
      printf("Case D: %f\n", sum[3]/100);
      break;
    case 4:
      printf("Case E: %f\n", sum[4]/100);
      break;
    case 5:
      printf("Case F: %f\n", sum[5]/100);
      break;
    }
  }
  return 0;
}
