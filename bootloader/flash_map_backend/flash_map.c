#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t, uint32_t

#include "flash_map.h"

// Simple flash map API for MCUboot
// This is needed to satisfy the linker

int flash_map_init(void)
{
    // No special initialization needed for STM32 internal flash
    return 0;
}

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
    (void)id;
    (void)fa;
    return 0;
}

void flash_area_close(const struct flash_area *fa)
{
    (void)fa;
}

// These are weak stubs.
// Real read/write/erase happens in flash_map_backend.c
