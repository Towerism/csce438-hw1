#pragma once

#include "common.h"

void process_command(int slave_socket, std::string command, std::string argument);
void read_command(int socket);
void process_commands(int master_socket);
