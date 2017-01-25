#pragma once

#include "common.h"

inline void print(std::string message) {
#ifdef DEBUG_MESSAGES
  printf(message.c_str());
  fflush(stdout);
#endif
}
