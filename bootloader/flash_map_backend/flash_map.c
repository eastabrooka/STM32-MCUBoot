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

