#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int pid = fork();
  if (pid == 0) 
    fprintf(stderr, "[child_pid: %d]yourself\n", pid);
  else
    fprintf(stderr, "[parent_pid: %d]kill\n", pid);
}
