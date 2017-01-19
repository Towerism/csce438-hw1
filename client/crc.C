#include "stdlib.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Improper arguments!" << endl;
    cerr << "Usage: crc <host name> <port number>" << endl;
    return 1;
  }
  int port = stoi(argv[2]);
  cout << "Attempting to connect to " << argv[1] << ":" << port << endl;

  struct addrinfo *available, *cn;
  struct addrinfo hints;
//  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  int s = getaddrinfo(argv[1], argv[2], &hints, &available);
  if (s != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }
  bool connected = false;
  for ( cn = available; cn != NULL; cn = cn->ai_next){
  int sfd = socket(cn->ai_family, cn->ai_socktype, cn->ai_protocol);
    if (sfd == -1)
        continue;
    if (connect(sfd, cn->ai_addr, cn->ai_addrlen) != -1){
      connected = true;
      break; /* Connected to socket! */
    }
    close(sfd);
  }
/*
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1){
    cerr << "Error creating a socket!" << endl;
    return 1;
  }
  if (connect(sockfd,p->ai_addr, p->ai_addrlen ) == -1){
    close(sockfd);
    cerr << "Error connecting to server!" << endl;
    return 1;
  }
*/
  if ( !connected ){
    cerr << "Failed to connect! You succcckkkkkk" << endl;
    return 1;
  }
  cout << "Connection established." << endl;
  cout << "Options: \n\tCREATE <name>, DELETE <name>, JOIN <name>" << endl;
  string action;
  cin >> action;
  return 0;
}
