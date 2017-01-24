#include "server_proc.h"
#include "chat_room.h"
#include "socket_helpers.h"

void process_command(int slave_socket, std::string command, std::string argument) {
  if (command == "CREATE") {
    create_chat_room(argument);
    write_to_socket(slave_socket, "Room '" + argument + "' created");
  } else if (command == "JOIN") {
    auto port = get_chat_room_port(argument);
    if (port >= 0)
      write_to_socket(slave_socket, "CTRL " + std::to_string(port));
    else
      write_to_socket(slave_socket, "Failed to join room " + argument);
  } else if (command == "DELETE") {
    if (delete_chat_room(argument) < 0)
      write_to_socket(slave_socket, "Failed to delete room " + argument);
    else
      write_to_socket(slave_socket, "Deleted room " + argument);
  } else  {
    write_to_socket(slave_socket, "Unknown command " + command);
  }
  close(slave_socket);
}

void read_command(int socket) {
  char data[MAX_BUFFER_LEN];
  if (read(socket, data, MAX_BUFFER_LEN) <= 0)
    return;
  std::string command(strtok(data, " "));
  std::string argument(strtok(nullptr, " "));
  process_command(socket, command, argument);
}

void process_commands(int master_socket) {
  listen(master_socket, MAX_SOCK_BACKLOG);
  while(true) {
    int slave_socket;
    printf("Awaiting request\n");
    fflush(stdout);
    if ((slave_socket = accept(master_socket, NULL, NULL)) != -1) {
      printf("Client connected\n");
      fflush(stdout);
      auto request_thread = std::thread(read_command, slave_socket);
      request_thread.detach();
    }
  }
}