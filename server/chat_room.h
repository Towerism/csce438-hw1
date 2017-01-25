#pragma once

#include "common.h"
#include "socket_list.h"

struct Chat_room {
  Chat_room(int port = 0, int socket = 0): port(port), master_socket(socket), active(true) {}
  Chat_room(const Chat_room& other)
    : port(other.port), master_socket(other.port), active(other.active.load()) {}
  Chat_room& operator=(const Chat_room& other) {
    port = other.port;
    master_socket = other.master_socket;
    active.store(other.active.load());
    return *this;
  }

  int port;
  int master_socket;
  std::atomic<bool> active;
  Socket_list sockets;
};

void connect_clients_to_chat_room(int master_socket, Chat_room& chat_room);
void handle_chat_client_outgoing(Chat_room& chat_room, int slave_socket);
void handle_chat_client_incoming(Chat_room& chat_room, int slave_socket);
void run_chat_room(Chat_room& chat_room);
void create_chat_room(std::string name);
int delete_chat_room(std::string name);
Chat_room get_chat_room(std::string name);
