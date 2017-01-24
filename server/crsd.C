#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

#include <iostream>
#define MAX_SOCK_BACKLOG 100
#define MAX_BUFFER_LEN 250

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
  return port;
}

void process_commands(int sockfd) {
  listen(sockfd, MAX_SOCK_BACKLOG);
  while(true) {
    int slave_socket;
    if ((slave_socket = accept(sockfd, NULL, NULL)) != -1) {
      printf("Client connected\n");
      fflush(stdout);
      if (fork() == 0) {
        // read/write
        char data[MAX_BUFFER_LEN];
        if (read(slave_socket, data, MAX_BUFFER_LEN) < 0)
          std::exit(EXIT_FAILURE);
        std::string command(strtok(data, " "));
        std::string argument(strtok(nullptr, " "));
        char response[MAX_BUFFER_LEN];
        memset(response, 0, MAX_BUFFER_LEN);
        if (command == "CREATE") {
          sprintf(response, "Creating room %s\n", argument);
          write(slave_socket, response, MAX_BUFFER_LEN);
        } else {
          sprintf(response, "Unknown command %s\n", command);
          write(slave_socket, response, MAX_BUFFER_LEN);
        }
        close(slave_socket);
      }

    }
  }
}

int main(int argc, char* argv[]) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return -1;
  auto addr = "127.0.0.1";
  auto port = bind_socket(master_socket, addr);
  printf("Connected at %s:%d\n", addr, port);
  fflush(stdout);
  process_commands(master_socket);
  close(master_socket);
}
