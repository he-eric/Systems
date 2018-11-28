#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>
#include<limits.h>
#include<dirent.h>
#include<ctype.h>
//#include "invertedIndex.h"

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

//LINKED LIST NODE THAT HOLDS THE FREQUENCY OF A WORD SHOWING UP IN A CERTAIN FILE
typedef struct file_node { 
	char* filename;
	int frequency;
	struct file_node* next;
} file_node;

//TREE NODE THAT HOLDS A WORD AND A FILE_NODE
typedef struct word_node {
	char* word;
	struct file_node* list;
	struct word_node* left;
	struct word_node* right;
} word_node;

//Allocates space for a given filename and creates a file_node
file_node* create_file_node(char* filename) {
	file_node* node = (file_node*)malloc(sizeof(file_node));
	node->filename = strdup(filename);
	node->frequency = 1;
	node->next = NULL;
	return node;

}

//Allocates space for the given word and creates a word_node
word_node * create_word(char * word,char* filename) {
  //printf("Create:%s\n", word);
	word_node * n = (word_node*)malloc(sizeof(word_node));
	n->word = word;
	n->list = create_file_node(filename);
	n->left = NULL; n->right = NULL;
	return n;
}

void free_linked_list(file_node * curr) {
  if (curr != NULL) {
    //printf("\tbegin\n");
    file_node * next = curr->next;
    //printf("\tset next\n");
    free(curr);
    //printf("\tfree\n");
    free_linked_list(next);
  }
}

void free_tree(word_node * curr) {
  if (curr == NULL) {
    return;
  }
  else {
    //printf("begin\n");
    word_node *left = curr->left;
    word_node *right = curr->right;
    //printf("set left right\n");
    free_linked_list(curr->list);
    //printf("free list\n");
    free(curr);
    //printf("free child\n");
    free_tree(left);
    free_tree(right);
  }
}


//Compares two strings giving precedence to letters, then numbers, then "."
//return -1 if str2 < str1, return 1 if str1 < str2, return 0 if str2 == str1
int compare_string(char *string1, char *string2){
  
  int i = 0;
  
  while( string1[i] != '\0'){
      
    //if string2 shorter than string1 return -1
    if(string2[i] == '\0') {
      printf("\t%s,  %s\n", string2, string1);
      return 1;
    }
    
    //if string1[i] a . and string2[i] is not a .
    if ( (string1[i] == 46 && string2[i] != 46) ) {
      printf("\t%s, %s\n", string2, string1);
      return 1;
    }
    
    //if string1[i] is not a . and string2[i] is .
    if ( (string1[i] != 46 && string2[i] == 46) ) {
      printf("\t%s, %s\n", string1, string2);
      return -1;
    }
    
    //if string1[i] a digit and string2[i] is not a digit
    if( (string1[i] >= 48 && string1[i] <=57) && (string2[i] > 57) ){
      printf("\t%s, %s\n", string2, string1);
      return 1;
    }
    
    //if string2[i] is a digit and string1[i] is not a digit
    if( (string2[i] >= 48 && string2[i] <=57) && (string1[i] > 57) ){
      printf("\t%s, %s\n", string1, string2);
      return -1;
    }
    
    //if both digits or both characters
    if(string1[i] < string2[i]) {
      printf("\t%s, %s\n", string1, string2);
      return -1;
    }
    
    if(string1[i] > string2[i]) {
      printf("\t%s, %s\n", string2, string1);
      return 1;
    }
    
    i++;
  }
  
  //if string2 longer than string1 return 1
  if(string2[i] != '\0'){
    printf("\t%s, %s\n", string1, string2);
    return -1;
  }
  
  printf("\t%s = %s\n", string2, string1);
  return 0;
  
} 

//Recursively inserts a word_node into the word tree so that the tree is in alphabetical order from left to right
word_node * insert_word(word_node * root, word_node * n) {
  if (!root) 
    return n;
  else if (compare_string(n->word, root->word) >= 0) {
    //printf("Right\n");
    root->right = insert_word(root->right, n);
  }
  else {
    //printf("Left\n");
    root->left = insert_word(root->left, n);
  }  
  return root;
}

//Searches through the binary search word tree for a node given word, returns 1 if found, 0 if not found
int search_tree(word_node* root,char* word){
	if(!root)
		return 0;
	word_node* ptr = root;
	while(ptr != NULL){
		if(compare_string(ptr->word,word) == 0){
			return 1;
		} else if(compare_string(ptr->word,word) < 0){
			ptr = ptr->right;
		} else {
			ptr = ptr->left;
		}
	}
	return 0;

}

//Insert a file_node into the beginning of a list 
file_node* insert_file_node(file_node* root, char* filename){

	if(!root)
		return root;

	file_node* new_node = (file_node*) malloc(sizeof(file_node));
	new_node->filename = filename;
	new_node->frequency = 1;
	new_node->next = root;

	return new_node;

}

//Prints the linked list inside of a word_node that contains file_nodes holding filenames and frequencies
void print_file_list(file_node* root){
	if(!root){
		printf("error\n");
		return;
	}
	file_node* ptr = root;
	while(ptr != NULL){
		printf("%s,%d -> ",ptr->filename,ptr->frequency);
		ptr = ptr->next;
	}
	printf("\n");

}

//Prints the word tree in in-order traversal fashion from left to right including each word_node's file_node list
void print_tree(word_node * root) {
  if (!root)
    return;
  print_tree(root->left);
  printf("%s\tLL: ", root->word);
  print_file_list(root->list);
  print_tree(root->right); 
}


//Given a word and filename, find the word inside the tree
//once the word is found, search through it's linked list for filename
//if filename is found, update it's frequency by 1
//if filename not found, call insert_file_node to create a new instance of a word in a file
//This function returns the word tree after updating the tree
word_node* update_tree(word_node* root,char* word,char* filename){
	
	if(!root)
		return root;

	//Starting at the root, search for the given word in the word tree
	word_node* ptr = root;
	while(ptr != NULL){
		if(compare_string(ptr->word,word) == 0){
			//If the program has found the correct word in the tree
			//Look through the word_node's linked list to find filename
			file_node* list_ptr = ptr->list;
			if(list_ptr == NULL){
				//if a linked list does not exist inside word_node
				ptr->list = create_file_node(filename);
				return root;
			}
			while(list_ptr != NULL){
				//Iteratively go through the linked list to find a given filename inside
				if(compare_string(list_ptr->filename,filename) == 0){
					//If the filename in the linked list matches the given filename to search for, increment frequency
					list_ptr->frequency++;
					return root;
				} else {
					//Iterate to the current pointer's next
					list_ptr = list_ptr->next;
				}
			}

			//After iterating through the entire linked list and not finding the filename,
			//create a new file_node containing the filename and insert it to the beginning of the list
			ptr->list = insert_file_node(ptr->list,filename);
			return root;


		} else if(compare_string(ptr->word,word) < 0){
			//Search right of the current pointer
			ptr = ptr->right;
		} else {
			//Search left of the current pointer
			ptr = ptr->left;
		}
	}
	return root;

}


//Given a path, this returns the last file name/directory name 
//ex given adir/bdir/boo, this returns boo
char* tokenize(char* path){

	char* token;
	char* last;
	token = strtok(path,"/");

	do {
		last = token;
		token = strtok(NULL,"/");
	} while(token != NULL);

	return last;

}

//Given a path, this function 
word_node* process_file(char* path,word_node* root){
	
	FILE* fp;

	//printf("path: %s\n", path);

	fp = fopen(path,"r");

	//If the file fails to open, print error
	if(fp == NULL){
		printf("%s\n",strerror(errno));
		exit(0);
	}

	//Given the path, tokenize to get the file name and copy it to filename
	char* tokenized_filename = tokenize(path);
	char* filename = strdup(tokenized_filename);
	//printf("filename: %s\n",filename);

	int j, start;

	//Make the filename lowercase
	for(j = 0;j<strlen(filename);j++){
		if(filename[j] >= 65 && filename[j] <= 90){
			filename[j] = tolower(filename[j]);
		}
	} 

	//Iterate through each letter of the file and tokenize the file into our desired words such that each word is defined
	//as any sequence of consecutive alphanumeric characters (a- z, A-Z, 0-9) starting with an alphabetic character.
	while(fgets(buffer,BUFFER_SIZE,fp)){
		j = 0;
		start = 0;
		while(buffer[j] != '\0'){
			//if the current character is not a letter 
		    if ((buffer[j] < 65 || buffer[j] > 90) && (buffer[j] < 97 || buffer[j] > 122)) {
		        //if the length of the word is 0, increment start but do not copy
		    	if (j-start == 0) 
			       start++;
			    //else if the current character is a digit and not the beginning of a word, continue adding to the current token
			   	else if( (buffer[j] >= 48 && buffer[j] <= 57) ){
			   		//simply does nothing in order to assure that digit is added to the token and that the token is not finished
			   	}
		      	//else copy the word between string[start,j] and insert it into the binary tree 
			    else {
			    	/*
			        allocate a string s with the length of j-start+1
			        the extra 1 is for the null terminator 
			        */
			        char * s = malloc(sizeof(char)*(j-start+1)); 
			        //copy the string starting at the index start and ending at j into s
			        strncpy(s, &buffer[start], j-start); 
			        s[j-start] = '\0';
			        
			        //Here I check if the word exists already in the word tree
			        //if it does then I find the word and update its linked list with the frequency and filename
			        //otherwise I have to create a new word_tree node and initialize the linked list inside it as well
			        if(!search_tree(root,s)){
			        	printf("create word: '%s'\t filename: %s\n",s,filename);
			        	root = insert_word(root, create_word(s,filename));
			    	} else {
			    		printf("update: '%s'\tfilename: %s\n",s,filename);
			    		root = update_tree(root,s,filename);

			    	}
			        //Modify index start to reflect the start of the next word
			      	start = j+1;
			    }
			}
			//if the current character is an uppercase letter, convert it to lowercase
			if(buffer[j] >= 65 && buffer[j] <= 90){
				buffer[j] = tolower(buffer[j]);
			}
			j++;
		}

		//idk if i need this
		/*
		if (j-start != 0) {
			printf("LAST TIME FOR FILE\n");
		    char * s = malloc(sizeof(char)*(j-start+1)); 
		    strncpy(s, &buffer[start], j-start);
		    s[j-start] = '\0';
		    if(!search_tree(root,s)){
	        	printf("create word2\n");
	        	root = insert_word(root, create_word(s,filename));
	    	} else {
	    		printf("update word2\n");

	    		root = update_tree(root,s,filename);
	    	
	    	}	    	
  		} */
  	
		

	}

	fclose(fp);

	return root;
}

//Iteratively prints out the filename and frequency of every file_node given a root of a linked list
void output_file(file_node* root,FILE* fp){
	if(!root)
		return;

	file_node* ptr = root;
	while(ptr != NULL){
		printf("output_file: filename: %s\n", ptr->filename);
		fprintf(fp, "\t\t<file name=\"%s\">%d</file>\n", ptr->filename,ptr->frequency);
		ptr = ptr->next;
	}

}

//Recursively print out each word, and calls output_file to print out it's linked list
//Prints in in-order traversal fasion such that the output is in alphabetical order to the given standards
void output_tree(word_node* root,FILE *fp){
	if(!root)
		return;

	output_tree(root->left,fp);

	fprintf(fp, "\t<word text=\"%s\">\n",root->word);
	output_file(root->list,fp);
	fprintf(fp, "\t</word>\n");

	output_tree(root->right,fp);


}

//Writes the beginning of the XML output to the given filename
//Calls output_tree to print out all the words in the word tree
void write_to_file(word_node * root,char* filename){

	print_tree(root);
	
	printf("write to file: %s\n", filename);
	FILE* fp;
	fp = fopen(filename,"w");
	if(fp == NULL){
		printf("%s\n",strerror(errno));
		return;
	}

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fp, "<fileIndex>\n");

	output_tree(root,fp);

	fprintf(fp, "</fileIndex>\n");
}

//The main driver of our program
/*
Given a path, this function will determine if it is a file or a directory
If the path is a file, call process_file on the path and return the output to the word tree
If the path is a directory, recursively traverse through the path
Through each recursive step:
	If the current path is a file, call process_file on the path
	If the current path is a directory, recrusively call to traverse through the directory of the given path
This function will return a word tree we will use to output the data
*/	
word_node* process_path(char *path, word_node* root) {
    DIR *dirp;
    struct dirent *dp;
    char child[PATH_MAX];

    //Could not open the path as a directory, this means the path must be a file
    if (!(dirp = opendir(path))) {
        //fprintf(stderr, "Could not open '%s': %s\n", path, strerror(errno));
    	root = process_file(path, root);
        return root;
    }   

    //Successfully opened the path as a directory
    //Iteratively go through each file or directory within the given path
    errno = 0;
    while ( (dp = readdir(dirp)) != NULL ) { 
        // printf("%s/%s\n", path, dp->d_name);

        if (dp->d_type == DT_REG) {
        	//If the current pointer points to a file, call process_file on the current path
            //printf("f %s/%s\n", path, dp->d_name);
            /* concatenate current path and subdir name */
            snprintf(child, PATH_MAX, "%s/%s", path, dp->d_name);
            root = process_file(child, root);

        } else if (dp->d_type == DT_DIR) {
        	//If the current pointer points to a directory, recursively call process_path on the current path
            /* concatenate current path and subdir name */
            snprintf(child, PATH_MAX, "%s/%s", path, dp->d_name);
           // printf("d %s\n", child);
            
            /* recursively print directory contents, except for . and .. */
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                process_path(child,root);
                //return root;
            }   

        } else {
        	//If the current pointer points to neither a file or directory print out a '?' 
            printf("? %s/%s\n", path, dp->d_name);
            exit(0);
        }   
    
        errno = 0;
    }   

    //print out any error
    if (errno != 0) {
        fprintf(stderr, "Error reading '%s': %s\n", path, strerror(errno));
        exit(0);
    }   

    closedir(dirp);

    return root;
}

//Given the root of a linked list, find it's length
int LL_length(file_node *root) {
  int len = 0;
  while (root != NULL) {
    len++;
    root = root->next;
  }
  return len;
}

//Given the root and length of a linked list, create an array that will store the data of the linked list
file_node *LL_to_array(file_node *root, int len) {
  if (len == 0)
    return NULL;
  
  int i = 0;
  file_node *array = malloc(sizeof(file_node)*len); 
  
  while (i < len) {
    //printf("%d < %d\n", i, len);
    array[i] = *root;
    root = root->next;
    i++;
  }
  return array;
}

//Given a linked list, array and the length of the array, put the data in the array into the linked list
file_node *array_to_LL(file_node *root, file_node *array[], int len) {
  int i = 0; file_node *tmp = root;
  while (i < len) {
    tmp->frequency = array[i]->frequency;
    tmp->filename = array[i]->filename;
    tmp = tmp->next;
    i++;
  }
  return root;
}

//Compare the file_nodes in an array
//Return -1 if n_1 frequency > n_2 frequency, return 1 if n_1 frequency < n_2 frequency
//otherwise if the frequencies are equal, return the comparison of the strings n_1 and n_2
int compare_filenodes(const void *n_1, const void *n_2) {
  file_node *n1 = *(file_node **)n_1;
  file_node *n2 = *(file_node **)n_2;
  if (n1->frequency > n2->frequency) {
   //printf("\t%d > %d\n", n1->frequency, n2->frequency);
    return -1;
  }
  else if (n1->frequency < n2->frequency) {
    //printf("\t%d < %d\n", n1->frequency, n2->frequency);
    return 1;
  }
  return compare_string(n1->filename, n2->filename);
}

//Sort the given linked list with precedence of frequency first, followed by alphabetical order of the filename given the standards
//Uses LL_Length(), LL_to_array(), array_to_LL(), compare_filenodes and qsort
void sort_filenodes(word_node *root) {
  if (root != NULL) {
    int len = LL_length(root->list);
    file_node *list = LL_to_array(root->list, len);
    file_node *array[len];
   
    int i = 0;
    for (i = 0; i < len; i++) {
      array[i] = &list[i];
    }
    
    qsort(array, len, sizeof(file_node *), compare_filenodes); 
    root->list = array_to_LL(root->list, array, len);
    //if (!root->list)
    //printf("\troot is null\n");
    //printf("\tanything\n");
    
    sort_filenodes(root->left);
    sort_filenodes(root->right);
  }
  return;
}

int main(int argc, char** argv){

	//Checks the given output filename and checks if it exists
	//If output filename exists, ask if user would like to overwrite
	//if yes, continue with program
	//if no, exit program
	struct stat statbuf;
	if( stat(argv[1],&statbuf) == 0){	
		char input = '\0';
		printf("File already exists. Would you like to overwrite %s (y/n)?: ",argv[1]);	
		scanf("%c",&input);	
		printf("\n");
		if(input == 'n' || input == 'N'){
			exit(1);
		}
		
	} 

	//Create the initial word tree
	word_node* root = NULL;


	//Call process_path on the given path
	root = process_path(argv[2],root);
	//sort the word tree after all the data has been entered
	sort_filenodes(root);
	//Write the word tree to the output file
	write_to_file(root,argv[1]);

	free_tree(root);
	//print_tree(root);
		
	//Test compare with filenames
	/*
	char string1[9] = "text.txt";
	char string2[8] = "tex.pdf";
	int d = compare_string(string1, string2);
	if (d == 0)
	  printf("%s,  %s\n", string1, string2);
	else if (d == -1) {
	  printf("%s,  %s\n", string2, string1);
	}
	else {
	  printf("%s,  %s\n", string1, string2);
	}
	*/
	//Test qsort
	/*
	int i;
	int len = LL_length(root->list);
	//printf("len:%d\n", len);
	
	file_node *list = LL_to_array(root->list, len);
	for (i = 0; i < len; i++) {
	  list[i] = list[i];
	  printf("[%s: %d]->", list[i].filename, list[i].frequency);
	}
	printf("NULL\n");

	qsort(list, len, sizeof(file_node *), compare_filenodes); 
	*/
	return 0;
}
