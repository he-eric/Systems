#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

char * recover_filename(FILE * f) {
  int fd;
  char fd_path[255];
  char * filename = malloc(255);
  ssize_t n;

  fd = fileno(f);
  sprintf(fd_path, "/proc/self/fd/%d", fd);
  n = readlink(fd_path, filename, 255);
  if (n < 0)
    return NULL;
  filename[n] = '\0';
  return filename;
}

void print_dir(const char *path)
{
  DIR *dirp;
  struct dirent *dp;
  char child[PATH_MAX];

  //Check if path is file or directory
  if (!(dirp = opendir(path))) {
    fprintf(stderr, "Could not open '%s': %s\n", path, strerror(errno));
    //path is file do our stuff
    FILE * fp = fopen(path, "r");
    printf("%s\n", recover_filename(fp));
    return;
  }   

  errno = 0;
  //path is directory
  while ( (dp = readdir(dirp)) != NULL ) { 
    // printf("%s/%s\n", path, dp->d_name);

    //Regular file
    if (dp->d_type == DT_REG) {

      printf("f %s/%s\n", path, dp->d_name);
      //Our stuff
      

    }

    //Directory
    else if (dp->d_type == DT_DIR) {

      /* concatenate current path and subdir name */
      snprintf(child, PATH_MAX, "%s/%s", path, dp->d_name);
      printf("d %s\n", child);
            
      /* recursively print directory contents, except for . and .. */
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
	print_dir(child);
      }   

    } else {
      printf("? %s/%s\n", path, dp->d_name);
    }   
    
    errno = 0;
  }   

  if (errno != 0) {
    fprintf(stderr, "Error reading '%s': %s\n", path, strerror(errno));
  }   

  closedir(dirp);
}

int main(int argc, char **argv) {

  print_dir("path.c");

  /*
  if (argc != 2) {
    printf("Not enough arguments\n");
  }
  */
  
  /*
  struct stat *statbuf = malloc(sizeof(struct stat));
  struct dirent *dr;
  DIR *drpt = opendir(".");
  char * dirname;
  */
  
  //Testing: Read all files and subdirectories
  /*
  if (drpt == NULL) {
    printf("Could not open directory\n");
    return 0;
  }

  while ( (dr = readdir(drpt)) != NULL ) {
    dirname = dr->d_name;
    //if ( strcmp( dirname, ".." ) && strcmp( dirname, ".") ) {
      
      printf("%s\n", dirname);
      
      stat(dr->d_name, statbuf);
      if ( (statbuf->st_mode & S_IFMT) == S_IFDIR) {
	printf("Directory: %s\n", dirname);
      }
      else {
	printf("Other    : %s\n", dirname);
      }
      
      
      //}
  }
  
  closedir(drpt);
  */

  return 0;

}
