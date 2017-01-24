#include "chat_room.h"
#include "socket_helpers.h"

std::map<std::string, Chat_room> chat_rooms;

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room) {
  int slave_socket;
  while(chat_room.active) {
    if ((slave_socket = accept(master_socket, NULL, NULL)) != -1) {
      chat_room.sockets_mtx->lock();
      chat_room.sockets.push_back(slave_socket);
      chat_room.sockets_mtx->unlock();
    }
  }
}

void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket) {
  while (true) {
    char data[MAX_BUFFER_LEN];
    if (read(slave_socket, data, MAX_BUFFER_LEN) <= 0)
      return;
    for (auto socket : chat_room.sockets) {
      if (slave_socket == socket)
        continue;
      write(socket, data, MAX_BUFFER_LEN);
    }
  }
}

void handle_chat_client_incoming(Chat_room& chat_room, int slave_socket) {
  fd_set readfds;
  while(true) {
    FD_ZERO(&readfds);
    for (auto socket : chat_room.sockets) {
      if (slave_socket == socket)
        continue;
      FD_SET(socket, &readfds);
    }
    int activity;
    int nsock = chat_room.sockets.size();
    select(nsock, &readfds, NULL, NULL, NULL);
    for (auto socket : chat_room.sockets) {
      if (FD_ISSET(socket, &readfds)) {
        char data[MAX_BUFFER_LEN];
        if (read(socket, data, MAX_BUFFER_LEN) <= 0)
          return;
        write(slave_socket, data, MAX_BUFFER_LEN);
      }
    }
  }
}

void run_chat_room(Chat_room& chat_room) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return;
  listen(master_socket, MAX_SOCK_BACKLOG);
  std::thread(connect_clients_to_chat_room, master_socket, std::ref(chat_room)).detach();
  int slave_socket;
  while(true) {
    if ((slave_socket = accept(master_socket, NULL, NULL)) != -1) {
      std::thread(handle_chat_client_incoming,
                  std::ref(chat_room), slave_socket).detach();
      std::thread(handle_chat_client_incoming,
                  std::ref(chat_room), slave_socket).detach();
    } else {
      break;
    }
  }
  close(master_socket);
}

void create_chat_room(std::string name) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return;
  auto addr = LOCALHOST;
  auto port = bind_socket(master_socket, addr);
  Chat_room room(port, master_socket);
  chat_rooms[name] = room;
  std::thread(run_chat_room, std::ref(chat_rooms[name])).detach();
}

int delete_chat_room(std::string name) {
  try {
    chat_rooms.at(name);
    chat_rooms.erase(name);
    return 0;
  } catch (std::out_of_range) {
    return -1;
  }
}

int get_chat_room_port(std::string name) {
  try {
    auto port = chat_rooms.at(name).port;
    return port;
  } catch (std::out_of_range) {
    return -1;
  }
}
