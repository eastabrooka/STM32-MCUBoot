#pragma once

/*
 * Flash device ID
 * (we only use internal flash memory = device 0)
 */
#define FLASH_DEVICE_INTERNAL_FLASH    0

/*
 * Flash area IDs
 */
#define FLASH_AREA_BOOTLOADER           0   // Bootloader area
#define FLASH_AREA_IMAGE_PRIMARY(idx)  (1 + (idx))   // Primary slot (image 0 -> 1)
#define FLASH_AREA_IMAGE_SECONDARY(idx) (FLASH_AREA_IMAGE_PRIMARY(idx) + 1) // Secondary slot (image 0 -> 2)
#define FLASH_AREA_IMAGE_SCRATCH       255 // No scratch area used (overwrite-only mode)

/*
 * Slot numbers
 */
#define BOOT_PRIMARY_SLOT    0
#define BOOT_SECONDARY_SLOT  1
