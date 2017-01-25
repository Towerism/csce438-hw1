#include "common.h"
#include "server_proc.h"
#include "socket_helpers.h"

int main(int argc, char* argv[]) {
  auto master_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (master_socket == -1)
    return -1;
  auto addr = LOCALHOST;
  auto port = bind_socket(master_socket, addr);
  printf("Chat server running at:\n\n\t %s:%d\n\n", addr, port);
  printf("Run 'pkill %s' to kill the server.\n", argv[0]);
  printf("Alternatively use 'ps' to get the PID\n\tand run 'pkill PID'.\n");
  fflush(stdout);
  if (fork() == 0) {
    process_commands(master_socket);
    close(master_socket);
  }
}
