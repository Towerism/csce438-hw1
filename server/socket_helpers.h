#pragma once

#include "common.h"

#define PORT_START 1024
#define LOCALHOST "127.0.0.1"

// bind a socket to address, returns the port
int bind_socket(int sockfd, const char* addr);

void write_to_socket(int socket, std::string message);
