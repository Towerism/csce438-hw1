#include "socket_list.h"

// close socket and remove it from internal list
void Socket_list::close(int socket) {
  mtx.lock();
  auto it = std::find(sockets.begin(), sockets.end(), socket);
  if (it != sockets.end())
    sockets.erase(it);
  mtx.unlock();
  ::close(socket);
}
// accept incoming connection adding it to internal list
// returns whether a connection was accepted successfully
int Socket_list::accept(int master_socket) {
  int slave_socket;
  if ((slave_socket = ::accept(master_socket, NULL, NULL)) > 0) {
    mtx.lock();
    sockets.push_back(slave_socket);
    mtx.unlock();
    return slave_socket;
  }
  return -1;
}
// forwards data to sockets in internal list, excluding from_socket
void Socket_list::forward_data(int from_socket, char* data) {
  mtx.lock();
  for (auto it = sockets.begin(); it != sockets.end(); ++it) {
    if (from_socket == *it)
      continue;
    write(*it, data, MAX_BUFFER_LEN);
  }
  mtx.unlock();
}
// collects data from all sockets except to_socket and writes to to_socket
void Socket_list::accrue_data(int to_socket) {
  fd_set readfds;
  int maxfd;
  FD_ZERO(&readfds);
  for (auto it = sockets.begin(); it != sockets.end(); ++it) {
    if (to_socket == *it)
      continue;
    FD_SET(*it, &readfds);
    maxfd = *it;
  }
  int activity;
  select(maxfd + 1, &readfds, NULL, NULL, NULL);
  for (auto it = sockets.begin(); it != sockets.end(); ++it) {
    if (FD_ISSET(*it, &readfds)) {
      char data[MAX_BUFFER_LEN];
      if (read(*it, data, MAX_BUFFER_LEN) <= 0)
        return;
      write(to_socket, data, MAX_BUFFER_LEN);
    }
  }
}
