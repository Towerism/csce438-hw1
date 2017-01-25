#pragma once

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include <stdexcept>
#include <map>
#include <mutex>

#define MAX_SOCK_BACKLOG 100
#define MAX_BUFFER_LEN 250
