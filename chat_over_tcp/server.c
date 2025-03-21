#include <netinet/ip.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  // give the port for the server
  if (argc < 2) {
    fprintf(stderr, "Port No. not provided. Program terminated.\n");
    exit(1);
  }

  int sockfd, newsockfd, portno, n;

  // this buffer will store the message to be sent and read.
  char buffer[255];
  // stored the server and client.
  struct sockaddr_in serv_addr, cli_addr;

  socklen_t clilen;

  // create a socket for IPv4 and TCP.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("Error in opening socket");
  }

  // Clear everything in serv_addr.
  bzero((char *)&serv_addr, sizeof(serv_addr));

  // Save the port from argv.
  portno = atoi(argv[1]);

  // Set the type of ip addr, _, port(in network short type).
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // Try to bind the socket to the server address.
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error("Error binding socket");
  }

  // Listen on the socket with maximum 5 connections.
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  // Accept a connection from the client and assign new file descriptor.
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

  if (newsockfd < 0) {
    error("Error accepting connection");
  }

  while (1) {
    // Clear the buffer.
    bzero(buffer, 255);
    // Read from the socket into the buffer.
    n = read(newsockfd, buffer, 255);
    if (n < 0) {
      error("Error reading message");
    }
    printf("Client: %s\n", buffer);

    // Clear the buffer.
    bzero(buffer, 255);
    // Read from stdin into the buffer.
    fgets(buffer, 255, stdin);
    // Write to the socket from the buffer.
    n = write(newsockfd, buffer, 255);
    if (n < 0) {
      error("Error writing message");
    }
    // If server sent Bye, break loop.
    int i = strncmp("Bye", buffer, 3);

    if (i == 0) {
      break;
    }
  }

  close(newsockfd);
  close(sockfd);
  return 0;
}
