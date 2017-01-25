#pragma once

#include "common.h"

class Socket_list {
public:
  Socket_list() = default;
  Socket_list(const Socket_list& other): sockets(other.sockets) {}
  Socket_list& operator=(const Socket_list& other) { sockets = other.sockets; }

  // close socket and remove it from internal list
  void close(int socket);

  // accept incoming connection adding it to internal list
  // returns the slave_socket when a connection was accepted successfully (-1 otherwise)
  int accept(int master_socket);

  // forwards data to sockets in internal list, excluding from_socket
  void forward_data(int from_socket, char* data);

  // returns the size of the internal list
  int size() const { return sockets.size(); }
private:
  std::mutex mtx;
  std::vector<int> sockets;
};
