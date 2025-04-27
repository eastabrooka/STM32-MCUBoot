#include "flash_map_backend/flash_map.h"
#include <mcuboot_config/mcuboot_config.h>


uint32_t flash_area_get_off(const struct flash_area *fa) {
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa) {
    return fa->fa_size;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa) {
    return fa->fa_device_id;
}

uint32_t flash_sector_get_off(const struct flash_sector *sector) {
    return sector->fs_off;
}

uint32_t flash_sector_get_size(const struct flash_sector *sector) {
    return sector->fs_size;
}

int flash_area_get_sector(const struct flash_area *fa, uint32_t off, struct flash_sector *sector) {
    if (off >= fa->fa_size) {
        return -1;
    }
    sector->fs_off = fa->fa_off + off;
    sector->fs_size = 4096; // Assuming 4KB sectors; adjust if wrong
    return 0;
}



int flash_area_id_from_multi_image_slot(int image_index, int slot) {
    (void)image_index;
    if (slot == 0) {
        return FLASH_AREA_IMAGE_PRIMARY(0);
    } else if (slot == 1) {
        return FLASH_AREA_IMAGE_SECONDARY(0);
    } else {
        return -1;
    }
}

int flash_area_id_from_image_slot(int slot) {
    if (slot == 0) {
        return FLASH_AREA_IMAGE_PRIMARY(0);
    } else if (slot == 1) {
        return FLASH_AREA_IMAGE_SECONDARY(0);
    } else {
        return -1;
    }
}

int flash_area_to_sectors(int fa_id, int *cnt, void *sectors) {
    const struct flash_area *fa;
    int rc = flash_area_open(fa_id, &fa);
    if (rc) return rc;

    struct flash_sector *out = (struct flash_sector *)sectors;

    out[0].fs_off = fa->fa_off;
    out[0].fs_size = fa->fa_size;
    *cnt = 1;

    flash_area_close(fa);
    return 0;
}
