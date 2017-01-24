#include "socket_helpers.h"

// bind a socket to address, returns the port
int bind_socket(int sockfd, const char* addr) {
  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  int port = PORT_START;
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
