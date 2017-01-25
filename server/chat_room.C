#include "chat_room.h"
#include "socket_helpers.h"
#include "printing.h"

std::map<std::string, Chat_room> chat_rooms;
std::string keep_alive_check = "CTRL KEEP ALIVE ?";

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room) {
  int slave_socket;
  listen(master_socket, MAX_SOCK_BACKLOG);
  while(chat_room.active) {
    print("Chatroom awaiting client connection\n");
    if (slave_socket = chat_room.sockets.accept(master_socket)) {
      print("Client connected to chatroom\n");
      std::thread(handle_chat_client_incoming,
                  std::ref(chat_room), slave_socket).detach();
      std::thread(handle_chat_client_outgoing,
                  std::ref(chat_room), slave_socket).detach();
    } else {
      print("Problem accepting client connection to chatroom\n");
    }
  }
  print("No longer connecting clients to chat room\n");
}

void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket) {
  char data[MAX_BUFFER_LEN];
  while (true) {
    if (read(slave_socket, data, MAX_BUFFER_LEN) <= 0) {
      chat_room.sockets.close(slave_socket);
      print("Client disconnected\n");
      return;
    }
    print("Forwarding message\n");
    chat_room.sockets.forward_data(slave_socket, data);
  }
}

void handle_chat_client_incoming(Chat_room& chat_room, int slave_socket) {
  while(true) {
    chat_room.sockets.accrue_data(slave_socket);
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
