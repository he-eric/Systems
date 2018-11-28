#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 50

int main(int argc, char **argv) {

  int fd, bytes;
  char buf[BUFSIZE];

  if (argc < 2) {
    return 1;
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("my error string");
    return 1;
  }

  bytes = read(fd, buf, BUFSIZE);

  printf("Read %d bytes\n", bytes);
  write(1, buf, bytes);
  printf("\n");
  
  return 0;

}
