#include "chat_room.h"
#include "socket_helpers.h"
#include "printing.h"

std::map<std::string, Chat_room> chat_rooms;

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room) {
  int slave_socket;
  listen(master_socket, MAX_SOCK_BACKLOG);
  while(chat_room.active) {
    print("Chatroom awaiting client connection\n");
    if ((slave_socket = accept(master_socket, NULL, NULL)) > 0) {
      print("Client connected to chatroom\n");
      chat_room.sockets.push_back(slave_socket);
      std::thread(handle_chat_client_incoming,
                  std::ref(chat_room), slave_socket).detach();
      std::thread(handle_chat_client_outgoing,
                  std::ref(chat_room), slave_socket).detach();
      continue;
    } else {
      print("Problem accepting client connection to chatroom\n");
      continue;
    }
    print("There was a serious error accepting client connection to chatroom\n");
  }
}

void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket) {
  while (true) {
    char data[MAX_BUFFER_LEN];
    if (read(slave_socket, data, MAX_BUFFER_LEN) <= 0)
      return;
    print("Receiving data from chatroom client\n");
    for (auto socket : chat_room.sockets) {
      if (slave_socket == socket)
        continue;
      print("Forwarding data to chatroom clients\n");
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
  std::thread(connect_clients_to_chat_room, chat_room.master_socket, std::ref(chat_room)).detach();
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
