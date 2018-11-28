#include <stdio.h>
#include <errno.h>
#include "mymalloc.h"

static char my_heap[MEMORY_SIZE];

/*
  mymalloc()
  Arguments: size_t size, char * FILE, int LINE
  Return: Allocated pointer or NULL
  
  Takes a requested payload and searches my_heap for a block that can satisfy the payload
  Too large of a payload results in a NULL. Coalesce will be called once to scan the heap for false
  fragmentation. It will resume search but if a NULL pointer is returned again a error message detailing 
  the specific LINE at which this occurs is printed
 */
void *mymalloc(size_t size, char * FILE, int LINE) {
  char * err_message = "Not enough space";
  
  //Initialized heap memory will always have 1 header
  //Thus total memory size - header size will be maximum largest payload
  if (size > 4998){
    error(FILE, LINE, err_message);
    return NULL;
  }
  
  void * p = mymalloc_helper(size);
  
  //P returned null
  //Did not find big enough block
  //Must coalesce
  if (!p) {
    coalesce();
    p = mymalloc_helper(size);
  }
  else 
    return p;
  
  //Depending on if p is NULL or not coalesce created a big enough block
  //If p is still NULL print an error message
  if(p == NULL){
    error(FILE,LINE,err_message);
  }
  return p;
}

/*
  mymalloc_helper
  Arguments: size_t size
  Return: allocated pointer or NULL

  Helper function for mymalloc that implements the searching. Calls split_block
 */
void *mymalloc_helper(size_t size) {
  char * current = my_heap;
  while ( current < my_heap+MEMORY_SIZE) {
    
    //Is a allocated block; increment header to next header
    if ( get_alloc( current ) )
      current = current +  get_size(current )+HEADER_SIZE;
    
    //Is deallocated block
    else {
      
      //If (requested payload+HEADER_SIZE) larger than the size in current header
      //Move to next header
      if( (size+HEADER_SIZE) > get_size(current))
	current = current +  get_size(current)+HEADER_SIZE;
      
      //If requested payload is equal to size in current header
      //Change block allocation from 0 to 1
      //Return our allocated block of memory
      else if (size == get_size(current)){
	PUT(current,PACK(size,1));
	return (void*)(current+HEADER_SIZE);
      } 

      //If requested payload is less than the size in current header
      //Split the block
      else {
	char* p = split_block(current,size);
	
	//If requested payload is less than the size in current header but split_block() returned null
	//The size in header was not large enough to accomodate the requested payload + header size
	//Move to next header
	if(p == NULL) {
	  current = current + get_size(current)+HEADER_SIZE;
	  continue;
	} 

	//Successfully split the block into two
	//Return p
	else {
	  return (void*)p;
	}
      }
    }
    
  }

  //End of while loop
  //Looked through every header in heap; not enough space
  return NULL;
}

/*
  myfree()
  Arguments: void *pointer, char *FILE, int LINE
  Return: Void

  Given a pointer, free() will attempt to deallocate that pointer by changing its header to allocation type.
  Errors occur if:
   1. Pointer not in heap
   2. Pointer is already freed
   3. Pointer is NULL
   4. Something went terribly wrong with how we implemented header increments
  Each error will print out the LINE and FILE at which the error occurred
*/
void myfree(void *pointer, char* FILE, int LINE){
  char* err_message1 = "Pointer not in heap";
  char* err_message2 = "Pointer already freed";
  char* err_message3 = "Pointer is NULL";
  char* err_message4 = "Something went terribly wrong";

  //Can not free NULL pointers
  if(pointer == NULL){
    error(FILE, LINE, err_message3);
    return;
  }
  
  //Can not free pointers not within heap
  if(pointer < (void*)my_heap || pointer > ((void*)my_heap + MEMORY_SIZE)){
    error(FILE, LINE,err_message1);
    return;
  }
  
  char* current = my_heap;
  while(current < my_heap+MEMORY_SIZE){
    
    //If current block is an allocated block
    if(get_alloc(current)){

      //If current block is the pointer we are looking for
      if((void*)(current+2) == pointer){
	
	//Found allocated block to free
	PUT(current,PACK(get_size(current),0));
	return;
      }
    }

    //If current block is a free block
    else {

      //If current block is the pointer we are looking for
      if((void*)(current+2) == pointer){

	//Trying to free a already freed block
	error(FILE, LINE, err_message2);
	return;
      }
    }
    
    //Current block was not the block we were looking for
    //Increment to next header
    current = current + get_size(current) + HEADER_SIZE;	
  }

  //If reached this point, something went terribly wrong
  error(FILE, LINE ,err_message4);
}

/*
  coalesce()
  Scan entire heap for adjacent free blocks
  Merges those free blocks
  Calls coalesce_helper
 */
void coalesce() {
  char * header1 = my_heap;
  char * header2 = header1 + HEADER_SIZE + get_size(header1);
  coalesce_helper(header1, header2);
}

/*
  coalesce_helper()
  Arguments: char *header1, char *header2
  Return: Void

  Recursive function that takes two headers and will attempt to merge those headers if they are free blocks
  Call the next two headers to merge
 */
void coalesce_helper(char * header1, char * header2) {
  
  //Base case
  //We have scanned entire heap
  if ( (header1 >= my_heap+MEMORY_SIZE-HEADER_SIZE) || (header2 >= my_heap+MEMORY_SIZE-HEADER_SIZE) ) {
    return;
  }
  
  unsigned short header1_alloc = get_alloc(header1);
  unsigned short header2_alloc = get_alloc(header2);
  unsigned short header1_size = get_size(header1);
  unsigned short header2_size = get_size(header2);
  char * header3 = header2 + header2_size + HEADER_SIZE;

  //If both headers are free blocks
  //merge them
  if ( (!header1_alloc) && (!header2_alloc) ) {
    unsigned short size = header1_size + header2_size + HEADER_SIZE;
    PUT(header1, PACK(size, 0));
    coalesce_helper(header1, header3);
  }
  
  //If both headers are not free blocks, and header2 is a allocated block
  //Move on to the next two headers after header2
  else if(header2_alloc){
    coalesce_helper(header3, header3+ get_size(header3) + HEADER_SIZE);
  }
  
  //If both headers are not free blocks, but header2 is a free block
  //Keep header2, increment to the header after header2
  else {
    coalesce_helper(header2, header3);
  }
}

/*
  printBlocks()
  Print information per block to test code
 */
void printBlocks(){

	char* current = my_heap;
	while(current < my_heap + MEMORY_SIZE){
		printf("%p\t%d\t%hu\n",current, get_alloc(current),get_size(current));
		current += get_size(current) + HEADER_SIZE;

	}
	printf("---------------\n");
}

/*
  initmem()
  Initalize heap to be 1 block of size 4998 and allocation type 0
 */
void initmem() {
	unsigned int * p = (unsigned int *)my_heap;
	PUT(p, 4998);
}

/*
  get_size()
  Arguments: char * header
  Return: Size of the block
 */
unsigned short get_size(char * header) {
	unsigned short size = GET(header);
	size = size & MASK;
	return size;
}

/*
  get_alloc()
  Arguments: char * header
  Return: Allocation type of the block
 */
unsigned short get_alloc(char * header) {
	unsigned short alloc = GET(header);
	alloc = alloc >> 15;
	return alloc;
}

/*
  split_block()
  Arguments: char * header, size_t size
 */
void* split_block(char * header, size_t size) {
	unsigned short old_size = get_size( header );
	
	//Impossible to split the block, not enough space
	if(size+HEADER_SIZE > old_size){
	  return NULL;
	}
	
	//Convert original header to type allocation 1 with size equal to requested payload
	PUT(header, PACK(size,1));
	
	//Create a new header
	char * new_header = header + size + HEADER_SIZE;
	
	//Convert new header to type allocation 0 with size equal to old size - size - header size
	PUT(new_header, PACK(old_size-size-HEADER_SIZE, 0));
	
	//Return the allocated block
	return header+2;
}

/*
  error()
  Arguments: char * FILE, int LINE, char * err_message
  Return: Void

  Prints out an error occuring at LINE in FILE
 */
void error(char * FILE, int LINE, char * err_message) {
  printf("File: %s Line: %d Message: %s\n", FILE, LINE, err_message);
}
