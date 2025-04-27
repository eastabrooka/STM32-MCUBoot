#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Full struct definition for flash_area
struct flash_area {
    uint8_t  fa_id;
    uint8_t  fa_device_id;
    uint16_t pad;
    uint32_t fa_off;
    uint32_t fa_size;
};

// Full struct definition for flash_sector
struct flash_sector {
    uint32_t fs_off;
    uint32_t fs_size;
};

// Function prototypes that MCUboot expects

int flash_area_open(uint8_t id, const struct flash_area **fa);
void flash_area_close(const struct flash_area *fa);
int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);
int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len);
int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len);
size_t flash_area_align(const struct flash_area *fa);
uint8_t flash_area_erased_val(const struct flash_area *fa);

uint32_t flash_area_get_off(const struct flash_area *fa);
uint32_t flash_area_get_size(const struct flash_area *fa);
uint8_t flash_area_get_device_id(const struct flash_area *fa);
int flash_area_get_sector(const struct flash_area *fa, uint32_t off, struct flash_sector *sector);
uint32_t flash_sector_get_off(const struct flash_sector *sector);
uint32_t flash_sector_get_size(const struct flash_sector *sector);

// Add these in flash_map.h (or another common header that loader.c includes)

int flash_area_to_sectors(int fa_id, int *cnt, void *sectors);
int flash_area_id_from_multi_image_slot(int image_index, int slot);
int flash_area_id_from_image_slot(int slot);



#ifdef __cplusplus
}
#endif
