#include "flash_map/flash_map.h"

// Simple flash map API for MCUboot
// This is needed to satisfy the linker

int flash_map_init(void)
{
    // No special init needed for STM32 internal flash
    return 0;
}