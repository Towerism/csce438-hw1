#include "socket_list.h"

void Socket_list::close(int socket) {
  mtx.lock();
  auto it = std::find(sockets.begin(), sockets.end(), socket);
  if (it != sockets.end())
    sockets.erase(it);
  mtx.unlock();
  ::close(socket);
}

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

void Socket_list::forward_data(int from_socket, char* data) {
  mtx.lock();
  for (auto it = sockets.begin(); it != sockets.end(); ++it) {
    if (from_socket == *it)
      continue;
    write(*it, data, MAX_BUFFER_LEN);
  }
  mtx.unlock();
}
