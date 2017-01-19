#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#define MAX_SOCK_BACKLOG 100

// bind a socket to address, returns the port
int bind_socket(int sockfd, const char* addr) {
  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  int port = 9999;
  int connection_result = 0;
  do {
    port += 1;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(addr);
    connection_result = bind(sockfd, (struct sockaddr *)&addr_in, sizeof(addr_in));
  } while (connection_result == -1 && errno == EADDRINUSE);
  if (connection_result == -1) {
    std::exit(EXIT_FAILURE);
  }
}

void process_commands(int sockfd) {
  listen(sockfd, MAX_SOCK_BACKLOG);
  while(true) {
    if (accept(sockfd, NULL, NULL) != -1) {
      printf("Client connected");
      fflush(stdout);
    }
  }
}

int main(int argc, char* argv[]) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return -1;
  auto addr = "127.0.0.1";
  auto port = bind_socket(master_socket, addr);
  printf("Connected at %s:%d", addr, port);
  fflush(stdout);
  process_commands(master_socket);
}
