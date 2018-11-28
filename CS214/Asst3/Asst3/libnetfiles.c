#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "libnetfiles.h"


#define BUF_SIZE 1024


char *hn;
int sfd, s;
struct addrinfo hints;
struct addrinfo *rp;
struct addrinfo *result;

int called_server_init = 0;

int set_connection() {
  /* Obtain address(es) matching host/port */
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;    /* Allow IPv4  */
  hints.ai_socktype = SOCK_STREAM; /* SOCK_STREAM socket */
  hints.ai_protocol = 0;          /* Any protocol */

  s = getaddrinfo(hn, "8520", &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    return -1;
  }

  /* getaddrinfo() returns a list of address structures.                        
     Try each address until we successfully connect(2).                         
     If socket(2) (or connect(2)) fails, we (close the socket                   
     and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype,
		 rp->ai_protocol);
    if (sfd == -1)
      continue;

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1){
      return 0;
    }
  }
  
  if (rp == NULL) {
    fprintf(stderr, "Could not connect\n");
    return -1;
  }
  return -1;
}

int time_out(int fd) {
  fd_set rfds;
  struct timeval tv;
  int retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  /* Wait up to fifteen seconds. */
  tv.tv_sec = 15;
  tv.tv_usec = 0;
  retval = select(fd+1, &rfds, NULL, NULL, &tv);

  if (retval == -1) {
    perror("select()");
    return -1;
  }
  else if (retval)
    //printf("Data is available now.\n");
    return 1;
    /* FD_ISSET(0, &rfds) will be true. */
  else
    printf("No data within 15 seconds.\n");
  return 0;
}


int netserverinit(char* hostname){

  /* Call set_connection to recieve a return value.
     If the address info is -1, that means there was an error with the connection
     therefore the server cannot be initialized. Otherwise the server will be initialized. */

  hn = hostname;
  
  int ret = set_connection();

  if (ret == -1) {
    printf("server not open\n");
    h_errno = HOST_NOT_FOUND;
    called_server_init = 0;
    return -1;
  }

  else {
    called_server_init = 1;
    freeaddrinfo(result);
    close(sfd);
    return 0;
  }
  
}

int netopen(const char *pathname, int flags){
  
  // printf("OPEN FUNC\n");

  /* If called_server_init == 0, then the server was never initialized for the client */
  if (called_server_init == 0) {
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  /* Call set_connection to return a value
     Check if the value is -1l in order to see if the connection is established. */

  int ret = set_connection();

  if (ret == -1){
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  
  /* Get the number of digits for flags and convert the integer value into a string.
     Doing this allows us to allocate proper space for the message to be sent to the server 
     and also concatinate the flag integer value into the message. */

  int num_digits;
  if (flags == 0) {
    num_digits = 2;
  }
  else {
    num_digits = (int) ((ceil (log10 ( (double) flags))) + 1);
  }  
  char string_flags[num_digits];  
  sprintf(string_flags, "%d", flags);
  
  /* Create the message */
  int len = 5 + strlen(pathname) + 1 + num_digits + 1;
  char message[len];
  strcpy(message, "open ");
  strcat(message, pathname);
  strcat(message, " ");
  strcat(message, string_flags);
  message[len] = '\0';
  //printf("message sent to server: %s\n",message);
  
  /* Send the message to the server. Print error if applicable and return -1. */
  if(write(sfd, message, strlen(message)) == -1){
    printf("%s\n",strerror(errno));
    return -1;
  }

  /* Wait for the server to return a file descriptor for the requested file. 
     Print any errors if applicable and return -1. */

  char buffer[BUF_SIZE];
  int nread;
  if(time_out(sfd) == 1){
    if( (nread = read(sfd, buffer , 100)) == 0){
      printf("%s\n",strerror(errno));
      return -1;
    }
  } else {
    errno = ETIMEDOUT;
    printf("%s\n",strerror(errno));
    return -1;
  }
  buffer[nread] = '\0';
  
  /* Convert the string value of the file descriptor recieved into an integer.
     If the file descriptor recieved was a -1, an error has occured and will print 
     and the function will return -1. Otherwise the file descriptor is valid. 
     We multiply that file descriptor by -10 in order to not confuse local file descriptors with
     file descriptors for files on the server side. Return the file descriptor. */

  //printf("fildes recieved from server for open: %s\n", buffer);
  int fildes;
  char* random;
  fildes = strtol(buffer,&random , 10);
  if(fildes == -1){
    if(time_out(sfd) == 1){
      nread = read(sfd, buffer, 10);
      buffer[nread] = '\0';
      errno = strtol(buffer,&random, 10);
      printf("%s\n",strerror(errno));
      return -1;
    } else {
      errno = ETIMEDOUT;
      printf("%s\n",strerror(errno));
      return -1;
    }
  }
  if(fildes > 0){
    fildes = fildes * -10;
  }
  
  //printf("fildes: %d\n",fildes);
  
  freeaddrinfo(result);
  close(sfd);
  //printf("--------\n");

  return fildes;

}

ssize_t netread(int fildes, void *buf, size_t nbyte){
  
  //printf("READ FUNC\n");

  /* If called_server_init == 0, then the server was never initialized for the client */
  if (called_server_init == 0) {
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  /* Call set_connection to return a value
     Check if the value is -1 in order to see if the connection is established. */

  int ret = set_connection();

  if(ret == -1){
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  
  /* Get the number of digits for fildes and nbytes and convert the integer values into strings.
     Doing this allows us to allocate proper space for the message to be sent to the server                                      
     and also concatinate the values into the message. */

  int fildes_digits = (int) ((ceil (log10 ( (double)(-1 * fildes)))) + 1);
  char string_fildes[fildes_digits];
  sprintf(string_fildes, "%d", fildes);

  int nbyte_digits = (int) ((ceil (log10 ( (double) nbyte))) + 1);
  char string_nbyte[nbyte_digits];
  sprintf(string_nbyte, "%d", nbyte);

  
  /* Create message */
  int len = 5 + fildes_digits + 1 + nbyte_digits + 1;
  char message[len];
  strcpy(message, "read ");
  strcat(message, string_fildes);
  strcat(message, " ");
  strcat(message, string_nbyte);
  message[len] = '\0';
  //printf("message sent to server from read: %s\n", message);

  /* Send the message to the server. Print error if applicable and return -1. */
  if(write(sfd, message, strlen(message)) == -1){
    printf("%s\n",strerror(errno));
    return -1;
  }
  
  /* Wait for server to return either a positive integer or -1. 
     If the value recieved is -1, that means there was an error.
     Otherwise the value should be the number of bytes to be read.
     This value should match nbyte. */

  char buffer[BUF_SIZE];
  char* random;
  int nread;
  if(time_out(sfd) == 1){
      if( (nread = read(sfd, buffer , 100)) == -1){
	printf("%s\n",strerror(errno));
      } 
  } else {
    errno = ETIMEDOUT;
    printf("%s\n",strerror(errno));
    return -1;
  }
  buffer[nread] = '\0';
 
  if(buffer[0] == '-'){
    if(time_out(sfd) == 1){
      nread = read(sfd, buffer, 10);
      buffer[nread] = '\0';
      errno = strtol(buffer,&random, 10);
      printf("%s\n",strerror(errno));
      return -1;
    } else {
      errno = ETIMEDOUT;
      printf("%s\n",strerror(errno));
      return -1;
    }
  }
 
  /* Wait for server to return n bytes from the file into buffer.
     Print any errors if applicable and return -1. 
     Otherwise copy the contents of buffer into the given buf parameter.
     Return number of bytes read. */

  int num_read;  
  num_read = strtol(buffer,&random,10);
  //printf("num read: %d\n",num_read);

  if(num_read <= 0){
    printf("read nothing\n"); 
    return -1;
  } 
  else {
    if(time_out(sfd) == 1){
      if(read(sfd, buffer, num_read) == -1){
	return -1;
      } else {
	//printf("read from file: %s\n",buffer);
      }
    } else {
      errno = ETIMEDOUT;
      printf("%s\n",strerror(errno));
      return -1;
    }
  }
  

  memcpy(buf,buffer,num_read + 1);

  close(sfd);
  freeaddrinfo(result);
  //printf("--------\n");
  return num_read;
}

/* fill_with_spaces takes a message that if has a length shorter than 100,
   will fill the the messages with spaces until the length is 100. */
   
char* fill_with_spaces(char* message){
  char* return_msg = message;
  int msg_len = strlen(return_msg);
  if(msg_len < 100){
    int len_needed = 100 - msg_len;
    int i;
    for(i = msg_len; i < 100; i++){
      return_msg[i] = ' ';
    }
    return_msg[100] = '\0';
  }
  return return_msg;
}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){

  //printf("WRITE FUNC\n");
  
  /* If called_server_init == 0, then the server was never initialized for the client */
  if (called_server_init == 0) {
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  /* Call set_connection to return a value
     Check if the value is -1 in order to see if the connection is established. */

  int ret = set_connection();

  if (ret == -1){
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  
  /* Get the number of digits for fildes and nbytes and convert the integer values into strings.                                 
     Doing this allows us to allocate proper space for the message to be sent to the server                                      
     and also concatinate the values into the message. */

  int fildes_digits = (int) ((ceil (log10 ( (double)(-1 * fildes)))) + 2);
  char string_fildes[fildes_digits];
  sprintf(string_fildes, "%d", fildes);

  int nbyte_digits = (int) ((ceil (log10 ( (double) nbyte))) + 1);
  char string_nbyte[nbyte_digits];
  sprintf(string_nbyte, "%d", nbyte);
  
  
  /* Create the message */
  char *message = malloc(101);
  strcpy(message, "write ");
  strcat(message, string_fildes);
  strcat(message, " ");
  strcat(message, string_nbyte);
  fill_with_spaces(message);
  //printf("%s\n",message);

  /* Send the message to the server. Print error if applicable and return -1. */
  if(write(sfd, message, strlen(message)) == -1){ 
    //Server failed to receive; treat as if server called close() before client did
    errno = ECONNRESET;
    printf("%s\n",strerror(errno));
    return -1;
  }
 
  /* Send the buffer that contains the data that is to be written into the file. Print error if applicable and return -1. */
  if(write(sfd,buf, nbyte) == -1){
    errno = ECONNRESET;
    printf("%s\n",strerror(errno));
    return -1;
  }

  char buffer[BUF_SIZE];

  /* Wait for the server to return the amount of bytes written. The number recieved is either a postive number or -1.
     If the number is -1, that means an error occured and the function will print an error and return -1.
     Otherwise, return the number of bytes written. */
  
  if ( time_out(sfd) == 1 ) {
  
    if(read(sfd, buffer, nbyte) == -1){
      printf("%s\n",strerror(errno));
      return -1;
    }
    
    int nread;
    char* random;
    
    if(buffer[0] == '-'){
      if( time_out(sfd) == 1 ){
	nread = read(sfd, buffer, 10);
	buffer[nread] = '\0';
	errno = strtol(buffer,&random, 10);
	printf("%s\n",strerror(errno));
	return -1;
      } else {
	errno = ETIMEDOUT;
	printf("%s\n", strerror(errno));
	return -1;
      }
    }

    int num_wrote;
    
    num_wrote = strtol(buffer,&random,10);
    //printf("num read: %d\n",num_wrote);

    close(sfd);
    freeaddrinfo(result);
    //printf("--------\n");
    return num_wrote;
  }
  else {
    errno = ETIMEDOUT;
    printf("%s\n", strerror(errno));
    return -1;
  }
}

int netclose(int fildes){

  // printf("CLOSE FUNC\n");

  /* If called_server_init == 0, then the server was never initialized for the client */
  if (called_server_init == 0) {
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  /* Call set_connection to return a value
     Check if the value is -1 in order to see if the connection is established. */

  int ret = set_connection();

  if (ret == -1) {
    h_errno = HOST_NOT_FOUND;
    return -1;
  }

  
  /* Get the number of digits for fildes and nbytes and convert the integer values into strings.                                 
     Doing this allows us to allocate proper space for the message to be sent to the server                                      
     and also concatinate the values into the message. */

  int fildes_digits = (int) ((ceil (log10 ( (double)(-1 * fildes)))) + 2);
  char string_fildes[fildes_digits];
  sprintf(string_fildes, "%d", fildes);

  /* Create the message */
  int len = 6 + fildes_digits + 1;
  char message[len];
  strcpy(message, "close ");
  strcat(message, string_fildes);
  message[len] = '\0';
  //printf("message: %s\n",message);

  /* Send the message to the server. Print error if applicable and return -1. */
  if(write(sfd, message, strlen(message)) == -1){
    printf("%s\n",strerror(errno));
    return -1;
  }

  char buffer[BUF_SIZE];
  
  /* Wait for the server to return a -1 or a 0. If the number recieved is -1, an error occurred and will return -1.
     Otherwise the file has been successfully closed and the function will return 0. */

  if(time_out(sfd) == 1){
    if(read(sfd, buffer, 100) == -1){
      printf("%s\n",strerror(errno));
      return -1;
    }
  
    //printf("buffer: %s\n",buffer);

    int nread;
    char* random;
    
    if(buffer[0] == '-'){
      if(time_out(sfd) == 1){
	nread = read(sfd, buffer, 10);
	buffer[nread] = '\0';
	errno = strtol(buffer,&random, 10);
	printf("%s\n",strerror(errno));
	return -1;
      } else {
	errno = ETIMEDOUT;
	printf("%s\n",strerror(errno));
	return -1;
      }
    }
  
    //printf("closed\n");
    //printf("--------\n");
    close(sfd);
    freeaddrinfo(result);

    return 0;
  } else {
    errno = ETIMEDOUT;
    printf("%s\n",strerror(errno));
    return -1;
  }
    
}



/*
int main(int argc, char *argv[])
{
  
  //size_t len;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  if(netserverinit(argv[1]) == -1){
    h_errno = HOST_NOT_FOUND;
    printf("%s\n",hstrerror(h_errno));
    exit(EXIT_FAILURE);
   }

  int index;
  
  int fd = netopen("testfile1.txt",O_RDONLY);
  char readbuf[20];
  char* writebuf = "mouse";
  netread(fd,readbuf,10);
  netclose(fd);
  fd = netopen("test2.txt",O_WRONLY);
  netwrite(fd,writebuf,5);
  netclose(fd);
  */

  //char buf[BUF_SIZE];
  //int fd, index;

/*  
  int pid = fork();
  
  if (pid != 0) {
    int fd = netopen("testfile2.txt",O_RDONLY);
    char buf[BUF_SIZE];
    netread(fd, buf, 10);
    netclose(fd);
    fd = netopen("testfile.txt",O_RDWR);
    char *buff = "test";
    netwrite(fd, buff, 4);
    netclose(fd);
    return 0;
  }
  else {
    int fd = netopen("testfile.txt", O_RDWR);
    char buf[BUF_SIZE];
    netread(fd, buf, 5);
    //buf[index] = '\0';
    printf("buf: %s\n", buf);
    char *buff = "bbb";
    netwrite(fd, buff, 3);
    netclose(fd);
    return 0;
  }
  
*/
  //netopen("testfile2.txt",O_RDONLY);
  //netclose(-60);
  //netclose(-60);
  //printf("---------\n");
  //netwrite(fd,"write",5);
  //netclose(fd);
  //printf("main func buf: %s\n",buf);
  /* Send command-line argument argv[2]
     read responses from server */
  
  //this is the message sent to server
  /*
  len = strlen(argv[2]) + 1;
  if (write(sfd, argv[2], len) != len) {                                      
    fprintf(stderr, "partial/failed write\n");                                
    exit(EXIT_FAILURE);                                                       
  }  */
 
  //close(sfd);
  //freeaddrinfo(rp);
 
  //exit(EXIT_SUCCESS);
  //}
