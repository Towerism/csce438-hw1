#include "common.h"
#include "server_proc.h"
#include "socket_helpers.h"

int main(int argc, char* argv[]) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return -1;
  auto addr = LOCALHOST;
  auto port = bind_socket(master_socket, addr);
  printf("Connected at %s:%d\n", addr, port);
  fflush(stdout);
  process_commands(master_socket);
  close(master_socket);
}
