#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node {
  char * word;
  struct node * left;
  struct node * right;
} node;

//Allocates space for the given word and creates a node
node * create(char * word) {
  //printf("Create:%s\n", word);
  node * n = malloc(sizeof(node));
  n->word = word;
  n->left = NULL; n->right = NULL;
  return n;
}

/*
Recursively inserts a word node into the tree
Base case: if root is null then return n
Recursive cases:
  if the string in current node precedes string in node n then insert to the right
  if the string in node n precedes string in current node then insert to the left
*/
node * insert(node * root, node * n) {
  if (!root) 
    return n;
  else if (strcmp(n->word, root->word) >= 0) {
    //printf("Right\n");
    root->right = insert(root->right, n);
  }
  else {
    //printf("Left\n");
    root->left = insert(root->left, n);
  }  
  return root;
}

/*
Recursively prints out the tree from left to right
Base case: if root is null print nothing
Recursive cases:
  Call print() on the left child
  Print the parent node when there are no more children beneath the left child 
  Call print() on the right child
*/
void print(node * root) {
  if (!root)
    return;
  print(root->left);
  printf("%s\n", root->word);
  print(root->right);
}

/*
Recursively frees nodes from memory
Base case: if root is null, return
Recursive cases:
  Call freeTree() on the left child
  Call freeTree() on the right child
  free the root node from memory
*/
void freeTree(node * root) {
  if (!root)
    return;
  freeTree(root->left);
  freeTree(root->right);
  free(root);
}

int main(int argc, char ** argv) {
  //Error if there is no input (1) or if there is more than 1 input (2)
  if (argc == 1){ 
    return 0;
  }
  if(argc > 2){
  	printf("Too many arguments.\n");
  	return 0;
  }

  /*
  Setup 
  start is the beginning index of the current word; j is the end index of the current word
  root is the binary tree which will hold our words
  string is the input string
  */
  int j = 0, start = 0;
  node * root = NULL;
  char * string = argv[1]; 
  

  //Iterate through each character of the input string until the null terminator is found
  while (string[j] != '\0' ) {
    //if the current character is not a letter 
    if ((string[j] < 65 || string[j] > 90) && (string[j] < 97 || string[j] > 122)) {
      //if the length of the word is 0, increment start but do not copy
      if (j-start == 0) 
	       start++;
      //else copy the word between string[start,j] and insert it into the binary tree 
      else {
        /*
        allocate a string s with the length of j-start+1
        the extra 1 is for the null terminator 
        */
        char * s = malloc(sizeof(char)*(j-start+1)); 
        //copy the string starting at the index start and ending at j into s
        strncpy(s, &string[start], j-start); 
        s[j-start] = '\0';
        
      	//printf("%p\n", s);
        root = insert(root, create(s));
        //Modify index start to reflect the start of the next word
      	start = j+1;
      }
    }
    j++;
  }
  //Copy last word
  if (j-start != 0) {
    char * s = malloc(sizeof(char)*(j-start+1)); 
    strncpy(s, &string[start], j-start);
    s[j-start] = '\0';
    root = insert(root, create(s));
    //root = insert(root, create(cpy(j, start, string)));
  }

  print(root);
  freeTree(root);
  return 0;
}
