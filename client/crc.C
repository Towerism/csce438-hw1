#include "stdlib.h"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define BUFFER_LENGTH 250

int connectTCP(char *host,int port){
  struct sockaddr_in serveraddr;
  struct hostent *hostp;
  int sd = -1;
  sd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(port);
  serveraddr.sin_addr.s_addr = inet_addr(host);

  if( serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE){
    hostp = gethostbyname(host);
    if(hostp == (struct hostent *)NULL){
      printf("Host not found --> ");
      return -1;
    }
    memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
  }
  int rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if (rc == -1){
    printf("Error while trying to connect!");
    return -1;
  }
  return sd;
}



int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Improper arguments!" << endl;
    cerr << "Usage: crc <host name> <port number>" << endl;
    return 1;
  }

  int port = stoi(argv[2]);
  cout << "Attempting to connect to " << argv[1] << ":" << port << endl;
  int sockfd = connectTCP(argv[1], port);
  //cout << "Socket " << sockfd << " in use" << endl;
  if (sockfd == -1){
    cerr << "Error connecting to provided host/port!" << endl;
    return 1;
  }
  cout << "Connection established." << endl;
  cout << "Options: \n\tCREATE <name>, DELETE <name>, JOIN <name>" << endl;
  while(true){
  /* Continue communicating with Master Socket.
 *   When a response for a JOIN request is received, exit loop.
 *   Join new channel, then pass messages there.
 * */
    string action;
    cin >> action;
  
    char buffer[BUFFER_LENGTH];
    strcpy(buffer, action.c_str());
    int send_result = write(sockfd, action.c_str(), sizeof(action.c_str()));
  
    int bytesReceived = 0;

    int rec_result = read(sockfd, buffer, BUFFER_LENGTH);
    
    // Check if reply is a CTRL reply that lets you join a lobby
    string s= string(buffer);
    if (s.substr(0,4) == "CTRL"){

    }
    else{
      cout << s << endl;
    }
  }
  close(sockfd);
 // Join chat room. Should be the same host, just a new port. 
  return 0;
}
