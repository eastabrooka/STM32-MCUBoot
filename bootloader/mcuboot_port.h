#pragma once

#include <stdint.h>
#include "flash_map_backend/flash_map_backend.h" // MCUboot needs this for flash_area API

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes that MCUboot expects (implemented in mcuboot_port.c)

// Open a flash area by ID
int flash_area_open(uint8_t id, const struct flash_area **fa);

// Close a flash area
void flash_area_close(const struct flash_area *fa);

// Read from a flash area
int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);

// Write to a flash area
int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len);

// Erase part of a flash area
int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len);

// Get flash area write alignment (e.g., 8 bytes for STM32L4)
size_t flash_area_align(const struct flash_area *fa);

// Return the erased value of flash (usually 0xFF)
uint8_t flash_area_erased_val(const struct flash_area *fa);

// Get sector layout of a flash area (optional, but needed for overwrite)
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);

// Map image slot index to flash area ID
int flash_area_id_from_image_slot(int slot);

#ifdef __cplusplus
}
#endif
