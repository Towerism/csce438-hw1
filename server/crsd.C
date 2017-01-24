#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

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

void write_to_socket(int socket, std::string message) {
  char response[MAX_BUFFER_LEN];
  memset(response, 0, MAX_BUFFER_LEN);
  strncpy(response, message.c_str(), MAX_BUFFER_LEN);
  write(socket, response, MAX_BUFFER_LEN);
}

void process_command(int slave_socket, std::string command, std::string argument) {
  if (command == "CREATE") {
    write_to_socket(slave_socket, "Creating room " + argument + "\n");
  } else if (command == "JOIN") {
    write_to_socket(slave_socket, "Joining room " + argument + "\n");
  } else if (command == "DELETE") {
    write_to_socket(slave_socket, "Deleting room " + argument + "\n");
  } else  {
    write_to_socket(slave_socket, "Unknown command " + command + "\n");
  }
}

void read_command(int socket) {
  char data[MAX_BUFFER_LEN];
  if (read(socket, data, MAX_BUFFER_LEN) < 0)
    std::exit(EXIT_FAILURE);
  std::string command(strtok(data, " "));
  std::string argument(strtok(nullptr, " "));
  process_command(socket, command, argument);
}

void process_commands(int master_socket) {
  listen(master_socket, MAX_SOCK_BACKLOG);
  while(true) {
    int slave_socket;
    printf("Awaiting request\n");
    fflush(stdout);
    if ((slave_socket = accept(master_socket, NULL, NULL)) != -1) {
      printf("Client connected\n");
      fflush(stdout);
      auto request_thread = std::thread(read_command, slave_socket);
      request_thread.detach();
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
