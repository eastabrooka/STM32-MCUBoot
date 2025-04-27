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

#ifdef __cplusplus
}
#endif
