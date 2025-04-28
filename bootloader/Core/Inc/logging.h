#pragma once

#include <stdio.h>

#define EXAMPLE_LOG(...)                            \
  do {                                              \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
  } while (0)
