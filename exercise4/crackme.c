/*
 * Exercise 4: crackme
 * gcc crackme.c -o crackme
 * usage: tcpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
static void error(char *msg) {
  perror(msg);
  exit(1);
}

static void gen_random_string(char *s, const int len) {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    int i = 0;
    for (; i < len; ++i) {
        s[i] = alphanum[random() % (sizeof(alphanum) - 1)];
    }
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
static long random_number(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

int main(int argc, char **argv) {
  int parentfd; /* parent socket */
  int childfd; /* child socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buffer */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  char *randomness = malloc(4096);

  srand(time(NULL));
  gen_random_string(randomness, 4096);

  /*
   * check command line arguments
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /*
   * socket: create the parent socket
   */
 parentfd = socket(AF_INET, SOCK_STREAM, 0);
  if (parentfd < 0)
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets
   * us rerun the server immediately after we kill it;
   * otherwise we have to wait about 20 secs.
   * Eliminates "ERROR on binding: Address already in use" error.
   */
  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
         (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));

  /* this is an Internet address */
  serveraddr.sin_family = AF_INET;

  /* let the system figure out our IP address */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* this is the port we will listen on */
  serveraddr.sin_port = htons((unsigned short)portno);

  /*
   * bind: associate the parent socket with a port
   */
  if (bind(parentfd, (struct sockaddr *) &serveraddr,
       sizeof(serveraddr)) < 0)
    error("ERROR on binding");

  /*
   * listen: make this socket ready to accept connection requests
   */
  if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
    error("ERROR on listen");

  /*
   * main loop: wait for a connection request, echo input line,
   * then close connection.
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    long start = random_number(4088);
    printf("Magic word is %.8s\n", &randomness[start]);

    /*
     * accept: wait for a connection request
     */
    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (childfd < 0)
      error("ERROR on accept");

    /*
     * gethostbyaddr: determine who sent the message
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server established connection with %s (%s)\n",
       hostp->h_name, hostaddrp);

    /*
     * read: read input string from the client
     */
    while(1) {
        /*
         * write: echo the input string back to the client
         */
        n = write(childfd, "Enter the magic string:\n", strlen("Enter the magic string:\n"));
        if (n < 0)
            error("ERROR writing to socket");

        bzero(buf, BUFSIZE);
        n = read(childfd, buf, BUFSIZE);
        if (n < 0)
            error("ERROR reading from socket");

        printf("server received %d bytes: %s", n, buf);

        if(!strncmp(buf, &randomness[start], 8)) {
            n = write(childfd, "Found the magic string!\n", strlen("Found the magic string!\n"));
            if (n < 0)
                error("ERROR writing to socket");
            break;
        }
    }

    close(childfd);
  }

  free(randomness);
}
