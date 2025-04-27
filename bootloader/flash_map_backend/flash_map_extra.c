#include "flash_map_backend.h"
#include "flash_map_backend/flash_map.h"

uint32_t flash_area_get_off(const struct flash_area *fa) {
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa) {
    return fa->fa_size;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa) {
    return fa->fa_device_id;
}

// These are dummy for now, safe to leave this way
int flash_area_get_sector(const struct flash_area *fa, uint32_t off, struct flash_sector *sector) {
    (void)fa;
    (void)off;
    (void)sector;
    return -1;
}

uint32_t flash_sector_get_off(const struct flash_sector *sector) {
    (void)sector;
    return 0;
}

uint32_t flash_sector_get_size(const struct flash_sector *sector) {
    (void)sector;
    return 0;
}

uint8_t flash_area_id_from_multi_image_slot(int image_index, int slot) {
    (void)image_index;
    (void)slot;
    return 0;
}
