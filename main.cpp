#include <stdint.h>
#include <algorithm>
#include <bitset>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

const int kMaxPendingConnection = 3;
const short kListenPort = 3456;

void DieWithError(const char *s) {
  perror(s);
  exit(EXIT_FAILURE);
}

void sendString(int sd, const char *s) {
  int length = strlen(s);
  if (send(sd, s, length, 0) != length) DieWithError("send() failed");
}

bool endsWith(const char *s, const char *postfix) {
  int len = strlen(s);
  int postfixLen = strlen(postfix);
  if (len < postfixLen) return false;
  if (strncmp(&s[len - postfixLen], postfix, postfixLen) == 0) return true;
  return false;
};

void HandleTCPClient(int sd) {
  int kReceiveBufferSize = 512;
  char echoBuffer[kReceiveBufferSize]; /* Buffer for echo string */
  int recvMsgSize;                     /* Size of received message */

  echoBuffer[kReceiveBufferSize - 1] = 0;
  while ((recvMsgSize = recv(sd, echoBuffer, kReceiveBufferSize - 1, 0)) > 0) {
    printf("%s", echoBuffer);
    if (endsWith(echoBuffer, "\n")) {
      break;
    }
    break;
  }
  if (recvMsgSize < 0) {
    DieWithError("recv() failed");
  }
  sendString(sd, "HTTP/1.1 200 OK\n\n");
  sendString(sd, "<!DOCTYPE html><h2>Hello!world.</h2> this is a test.\n");
  puts("connection closed");
  close(sd);
  return;
}

int main(int argc, char *argv[]) {
  int servSock;                    /* Socket descriptor for server */
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in echoServAddr; /* Local address */
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned int clntLen;            /* Length of client address data structure */

  /* Create socket for incoming connections */
  if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket() failed");
    exit(EXIT_FAILURE);
  }

  /* Construct local address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
  echoServAddr.sin_family = AF_INET;              /* Internet address family */
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  echoServAddr.sin_port = htons(kListenPort);       /* Local port */

  /* Bind to the local address */
  if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) <
      0) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }

  /* Mark the socket so it will listen for incoming connections */
  if (listen(servSock, kMaxPendingConnection) < 0) {
    perror("listen() failed");
    exit(EXIT_FAILURE);
  }

  for (;;) /* Run forever */
  {
    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);

    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *)&echoClntAddr,
                           &clntLen)) < 0) {
      perror("accept() failed");
      exit(EXIT_FAILURE);
    }

    /* clntSock is connected to a client! */

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    HandleTCPClient(clntSock);
  }
  /* NOT REACHED */

  return 0;
}
