#include "stdlib.h"

#include <iostream>
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
using namespace std;

#define BUFFER_LENGTH 250
#define STDIN 0

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

void chatroom(char *host, int port, string name);

int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Improper arguments!" << endl;
    cerr << "Usage: crc <host name> <port number>" << endl;
    return 1;
  }
  string userName = "Anonymous";
  int port = stoi(argv[2]);
  cout << "Attempting to connect to " << argv[1] << ":" << port << endl;
  int sockfd = connectTCP(argv[1], port);
  //cout << "Socket " << sockfd << " in use" << endl;
  if (sockfd == -1){
    cerr << "Error connecting to provided host/port!" << endl;
    return 1;
  }
  cout << "Connection established." << endl;
  cout << "Options: \n\tCREATE <name>, DELETE <name>, JOIN <name>, USER <name>" << endl;
  int chatroom_port = -1;
  while(true){
  /* Continue communicating with Master Socket.
 *   When a response for a JOIN request is received, exit loop.
 *   Join new channel, then pass messages there.
 * */
    string command, name;
    cin >> command >> name;
    transform(command.begin(), command.end(), command.begin(), ::toupper);
//command = command.toupper();
    if(command == "CREATE" || command == "DELETE" || command == "JOIN"){
       // Send Message
       char buffer[BUFFER_LENGTH];
       memset(buffer, 0, sizeof(buffer));
       string message_to_send = string(command + " " + name);
       strncpy(buffer, message_to_send.c_str(), sizeof(buffer));
       int send_result = write(sockfd, buffer, sizeof(buffer));
	cout << "Result of sending: " << send_result << endl;
       cout << "********    Message sent, awaiting result  *******" << endl;  
	cout << "Message: |" << message_to_send << "|" << endl;
       int bytesReceived = 0;

       int rec_result = read(sockfd, buffer, BUFFER_LENGTH);
    
       // Check if reply is a CTRL reply that lets you join a lobby
       string s= string(buffer,strlen(buffer));
       
       if (s.substr(0,4) == "CTRL"){
 	  // Assign chatroom_port
 	  string port_str = s.substr(5);
	  // First number is port, Second is # of people in room
	  //    seperated by a space.
	  auto split_location = port_str.find(" ");
	  string port = port_str.substr(0, split_location);
	  string user_count = port_str.substr(split_location + 1);
	  cout << "Trying to join the chat room on port: ";
          cout << "|" << port << "|" << endl;
          chatroom_port = stoi(port);
	  cout << "There are currently " << user_count << " users in this chatroom." << endl;
	  if(userName == "Anonymous")
		userName += user_count;
          break; 
       }
       else{
         cout <<"|" << s << "|" << endl;
	close(sockfd);
     	sockfd = connectTCP(argv[1], port);
       }
   
    }
    else{
 	if (command == "USER"){
	   userName = name;
	}
	else{
	       // Invalid input
	       cout << "Invalid input, please use the format specified by the options message" << endl;
	}
    }
  }
  close(sockfd);
 // Join chat room. Should be the same host, just a new port. 
//  sockfd = connectTCP(argv[1], chatroom_port);
//  system("clear");
  cout << "\n\n\n\n\n\n " << endl;
  chatroom(argv[1], chatroom_port, userName);
  return 0;
}



void chatroom(char *host, int port, string name){
  // Connect to chatroom
  int sockfd = connectTCP(host, port);
  int maxfd = (sockfd > STDIN)?sockfd:STDIN;
  string control_msg = "TXT " + name + ": ";
  int control_length = control_msg.length();
  while(true){
      // Loop while client is still connected to chatroom
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET( STDIN, &fds);
      FD_SET( sockfd, &fds);

      // Check who has input: stdin or socket
      int result  = select(maxfd + 1, &fds, NULL, NULL, NULL); // No timeout, no error sockets, no write sockets
      if (result == -1){
	// Some error occurred!
	cerr << "Error with select in chatroom function!" << endl;
        return;
      }
      if(FD_ISSET(STDIN, &fds)){
          string userInput;
          std::getline(std::cin, userInput);
          userInput = userInput + "\n";
          do{
              char buffer[BUFFER_LENGTH];
 	      memset(buffer, 0, sizeof(buffer));
              strncpy(buffer, string(control_msg + userInput).c_str(), sizeof(buffer));
		// Set the last character to null byte so it is a null terminated string
	      memset(buffer + BUFFER_LENGTH-1, 0, 1);
              int send_result = write(sockfd, buffer, sizeof(buffer));
//              cout << "Message: |" << buffer << "|" << endl;
              if(userInput.length() + control_length > BUFFER_LENGTH)
                     userInput = userInput.substr(BUFFER_LENGTH - control_length - 1);
	      else
		break;
	  }while(true);

      }
      if(FD_ISSET(sockfd, &fds)){
          char buffer[BUFFER_LENGTH];
   	  memset(buffer, 0, sizeof(buffer));
	  int rec_result = read(sockfd, buffer, BUFFER_LENGTH);
	  if(strncmp(buffer, "TXT ", 4) == 0){
	      // Is a TXT message and should display
	      cout << buffer + 4;
          }
          else{
             // Is a CTRL message and should respond
             char buffer2[BUFFER_LENGTH];
 	     memset(buffer2, 0, sizeof(buffer));
             strncpy(buffer, "Keep Alive", sizeof(buffer));
             int send_result = write(sockfd, buffer, sizeof(buffer));
          }
      }
  }
}
