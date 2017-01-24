#pragma once

#include "common.h"

struct Chat_room {
  Chat_room(int port = 0, int socket = 0): port(port), master_socket(socket), active(true) {}
  ~Chat_room() { /*delete sockets_mtx; */}

  int port;
  int master_socket;
  bool active;
  std::vector<int> sockets;
};

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room);
void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket);
void handle_chat_client_incoming(Chat_room& chat_room, int slave_socket);
void run_chat_room(Chat_room& chat_room);
void create_chat_room(std::string name);
int delete_chat_room(std::string name);
int get_chat_room_port(std::string name);
