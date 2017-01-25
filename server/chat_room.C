#include "chat_room.h"
#include "socket_helpers.h"
#include "printing.h"

std::map<std::string, Chat_room> chat_rooms;
std::string keep_alive_check = "CTRL KEEP ALIVE ?";

void keep_client_alive(Chat_room& chat_room, int slave_socket) {
  fd_set readfds;
  while(true) {
    print("Sending keep alive request to client\n");
    write_to_socket(slave_socket, keep_alive_check);
    FD_ZERO(&readfds);
    FD_SET(slave_socket, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    print("Waiting 1 second for keep alive response from client\n");
    select(slave_socket + 1, &readfds, NULL, NULL, &timeout);
    if (FD_ISSET(slave_socket, &readfds)) {
      print("Client is alive\n");
      char data[MAX_BUFFER_LEN];
      read(slave_socket, data, MAX_BUFFER_LEN);
    } else {
      auto it = std::find(chat_room.sockets.begin(), chat_room.sockets.end(), slave_socket);
      chat_room.lock();
      chat_room.sockets.erase(it);
      chat_room.unlock();
      close(slave_socket);
      print("Client disconnected\n");
      return;
    }
  }
}

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room) {
  int slave_socket;
  listen(master_socket, MAX_SOCK_BACKLOG);
  while(chat_room.active) {
    print("Chatroom awaiting client connection\n");
    if ((slave_socket = accept(master_socket, NULL, NULL)) > 0) {
      print("Client connected to chatroom\n");
      chat_room.lock();
      chat_room.sockets.push_back(slave_socket);
      chat_room.unlock();
      std::thread(handle_chat_client_incoming,
                  std::ref(chat_room), slave_socket).detach();
      std::thread(handle_chat_client_outgoing,
                  std::ref(chat_room), slave_socket).detach();
      std::thread(keep_client_alive,
                  std::ref(chat_room), slave_socket).detach();
    } else {
      print("Problem accepting client connection to chatroom\n");
    }
  }
  print("No longer connecting clients to chat room\n");
}

void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket) {
  while (true) {
    char data[MAX_BUFFER_LEN];
    if (read(slave_socket, data, MAX_BUFFER_LEN) <= 0)
      return;
    print("Receiving data from chatroom client\n");
    for (auto it = chat_room.sockets.begin(); it != chat_room.sockets.end(); ++it) {
      if (slave_socket == *it)
        continue;
      print("Forwarding data to chatroom clients\n");
      write(*it, data, MAX_BUFFER_LEN);
    }
  }
  print("No longer handling outgoing messages for client\n");
}

void handle_chat_client_incoming(Chat_room& chat_room, int slave_socket) {
  fd_set readfds;
  int maxfd;
  while(true) {
    FD_ZERO(&readfds);
    for (auto it = chat_room.sockets.begin(); it != chat_room.sockets.end(); ++it) {
      if (slave_socket == *it)
        continue;
      FD_SET(*it, &readfds);
      maxfd = *it;
    }
    int activity;
    select(maxfd + 1, &readfds, NULL, NULL, NULL);
    for (auto it = chat_room.sockets.begin(); it != chat_room.sockets.end(); ++it) {
      if (FD_ISSET(*it, &readfds)) {
        char data[MAX_BUFFER_LEN];
        if (read(*it, data, MAX_BUFFER_LEN) <= 0)
          return;
        write(slave_socket, data, MAX_BUFFER_LEN);
      }
    }
  }
  print("No longer handling incoming messages for client\n");
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
    auto room = chat_rooms.at(name);
    room.active = false;
    chat_rooms.erase(name);
    return 0;
  } catch (std::out_of_range) {
    return -1;
  }
}

Chat_room get_chat_room(std::string name) {
  try {
    return chat_rooms.at(name);
  } catch (std::out_of_range) {
    return Chat_room(-1);
  }
}
