#pragma once

#include <stddef.h>

static inline void *os_malloc(size_t size) { return NULL; }
static inline void os_free(void *ptr) {}
