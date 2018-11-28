#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

#define BACKLOG 5

void server_open(int sfd, char *filename, int flags);
void server_read(int sfd, int fd, void *buf, int count);
void server_write(int sfd, int fd, void *buf, int count);
void server_close(int sfd, int fd);


struct connection {
  struct sockaddr_storage addr;
  socklen_t addr_len;
  int fd;
};

int server(char *port);
void *echo(void *arg);

int main(int argc, char **argv)
{
  (void) server("8520");
  return EXIT_SUCCESS;
}


int server(char *port)
{
  struct addrinfo hint, *result, *r;
  struct connection *con;
  int rc, sfd;
  pthread_t tid;

  // initialize hints
  memset(&hint, 0, sizeof(struct addrinfo));
  hint.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hint.ai_socktype = SOCK_STREAM;

  // get socket and address info for listening port
  rc = getaddrinfo(NULL, port, &hint, &result);
  if (rc != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
    return -1;
  }

  // attempt to create socket
  for (r = result; r; r = r->ai_next) {
    sfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
    if (sfd == -1) {
      continue;
    }

    // if we were able to create the socket, try to set it up for
    // incoming connections
    if ((bind(sfd, r->ai_addr, r->ai_addrlen) == 0) &&
	(listen(sfd, BACKLOG) == 0)) {
      break;
    }

    // unable to set it up, so try the next addrinfo
    close(sfd);
  }

  freeaddrinfo(result);

  if (!r) {
    // we reached the end of result without successfuly binding a socket
    fprintf(stderr, "Could not bind\n");
    return -1;
  }

  // at this point sfd is bound and listening
  
  con = malloc(sizeof(struct connection));
  for (;;) {
    
    //printf("Waiting for connection!\n");
    con->addr_len = sizeof(struct sockaddr_storage);
    con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
    if (con->fd == -1) {
      perror("accept");
      continue;
    }
    
    //printf("Connection accepted!\n");
    rc = pthread_create(&tid, NULL, echo, con);
    if (rc != 0) {
      fprintf(stderr, "Unable to create thread: %d\n", rc);
      close(con->fd);
      continue;
    }

    pthread_detach(tid);
   
    //printf("connection ended...\n");
    
    con = malloc(sizeof(struct connection));
  }

  // never reach here
  return 0;
}

void *echo(void *arg)
{

  //Read 100bytes into buf
  int nread, nwrite;
  char buf[101];
  struct connection *c = (struct connection *) arg;
  
  // While the client doesnt close off the connection continue reading forever from the client
  while ( (nread = read(c->fd, buf, 100)) != 0) {
    //printf("Connected to socket fd: %d...waiting for message...\n", c->fd);
  if (nread == -1) {
    printf("READ ERROR\n");
    return;
  }
  buf[nread] = '\0';
  //printf("Message from client: %s\n", buf);

  /* 
     See what the buffer says
     printf("%s\n", buf);

     Tokenize the string by spaces
     Assuming the client will send a message accordingly
     Ex: "open filename flags" -> 3 tokens
     Ex: "read fd bytes"       -> 3 tokens
     Ex: "write fd buf bytes"  -> 4 tokens
     Ex: "close fd"            -> 2 tokens 
  */
  
  // Determine netcommand from first token
  char *token = strtok(buf, " ");
  if (strcmp(token, "open") == 0) {

    // Expect 2 more tokens
    // Expect next token to be filename
    token = strtok(NULL, " ");
    char *filename = strdup(token);
    
    // Expect last token to be flags; convert from string to int
    token = strtok(NULL, " ");
    char *ptr;
    int flags = strtol(token, &ptr, 10);

    // Call open
    
    //printf("open( %s, %d )\n", filename, flags);

    server_open(c->fd, filename, flags);
  }
  else if (strcmp(token, "read") == 0) {

    // Expect 3 more tokens
    // Expect next token to be file descriptor
    token = strtok(NULL, " ");
    char *ptr;
    int fd = strtol(token, &ptr, 10);
    if (fd != -1)
      fd = fd / -10;

    // Expect next token to be buffer

    // Expect last token to be expected number of bytes read
    token = strtok(NULL, " ");
    int bytes = strtol(token, &ptr, 10);
    
    // Call read
    //printf("read( %d, buffer, %d)\n", fd, bytes);
    void *buf = malloc(bytes);
    server_read(c->fd, fd, buf, bytes);

  }
  else if (strcmp(token, "write") == 0) {
    
    // Expect 3 tokens
    // Expect next token to be file descriptor
    token = strtok(NULL, " ");
    char *ptr;
    int fd = strtol(token, &ptr, 10);
    if (fd != -1)
      fd = fd / -10;

    // Expect next token to be # of bytes
    token = strtok(NULL, " ");
    int bytes = strtol(token, &ptr, 10);

    // Read buffer from client
    void *buf = malloc(bytes);
    nread = read(c->fd, buf, bytes);
    if (nread == -1) { 
      printf("%s\n", strerror(errno));
      return;
    }
  
    // Call write
    //printf("write( %d, buffer, %d)\n", fd, bytes);
    server_write(c->fd, fd, buf, bytes);
    
  }
  else if (strcmp(token, "close") == 0) {
    
    // Expect one more token
    // Expect token to be file descriptor
    token = strtok(NULL, " ");
    char *ptr;
    int fd = strtol(token, &ptr, 10);
    if (fd != -1)
      fd = fd / -10;

    //printf("close(%d)\n", fd);

    // Call close
    server_close(c->fd, fd);
  }
  else {
    //printf("other\n");
  }
  
  }
  
  close(c->fd);
  //printf("Closed socket\n");
  return NULL;
}

void message(int sfd, int err) {
  // Convert from int to string
  // First find number of digits 
  int num_digits;

  
  if (err < 0)
    num_digits = ((int)( (ceil( log10( (double) (abs(err))) ) ) ) )+ 2;
  else if (err == 0) {
    num_digits = 2;
  }
  else 
    num_digits = ((int)( (ceil( log10( (double)err) ) ) ) ) + 1;

  // Then convert 
  char num[num_digits];
  
  sprintf(num, "%d", err);

  // Finally send the int to client
  if ( write(sfd, num, num_digits) < 0 )
    printf("[SERVER] message: writing to socket\n");
}

void server_open(int sfd, char *pathname, int flags) {
  
  // Check flags
  // Send client error message if flag not 0,1,2
  
  if (flags < 0 || flags > 2) {
    
    message(sfd, -1);
    
    errno = ENOTSUP;
    printf("%s\n", strerror(errno));
    message(sfd, ENOTSUP);
    return;
  }
  

  // Open file
  // Send client the return value of open
  int ret = open(pathname, flags);
  message(sfd, ret);
  
  // Send errno if return value is negative
  if (ret < 0)
    message(sfd, errno);

  // Testing purposes
  //if (ret > 0) {
  //  printf("Closed file\n");
  //  close(ret);
  //}
}

void server_close(int sfd, int fd) {
  // Close file
  int ret = close(fd);

  // Send client the return value of close
  message(sfd, ret);

  // Send errno if return value is negative
  if (ret < 0)
    message(sfd, errno);
}

void server_read(int sfd, int fd, void *buf, int count) {
  
  // Read file 
  int ret = read(fd, buf, count);
  

  // Send client the return value of read
  message(sfd, ret);
  
  // If the return value is positive send the message read from file
  if (ret >= 0) {
    //printf("Message sent to client: %s\n", (char *)buf);
    if ( write(sfd, buf, ret) < 0)
      printf("[SERVER] server_read: writing to socket\n");
  }

  // If the return value is negative send errno
  else {
    message(sfd, errno);
  }

}

void server_write(int sfd, int fd, void *buf, int count) {
  
  // Call write
  //printf("fd: %d\n", fd);
  int ret = write(fd, buf, count);
  //printf("ret: %d\n", ret);
  
  // Send return value of write to client
  message(sfd, ret);


  //printf("ret sent\n");
  // Send errno if ret value is negative
  if (ret < 0) {
    message(sfd, errno);
    printf("%s\n", strerror(errno));
  }
}


