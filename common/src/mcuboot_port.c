#include <string.h>
#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "flash_map_backend/flash_map_backend.h"
#include "os/os_malloc.h"
#include "sysflash/sysflash.h"
#include "mcuboot_config/mcuboot_logging.h"
#include "mcuboot_config/mcuboot_assert.h"

#define FLASH_BASE_ADDRESS 0x0000000U
#define FLASH_BOOTLOADER_SIZE (128 * 1024) // 128KB
#define FLASH_APP_SLOT_SIZE (256 * 512)   // Adjust if needed
#define FLASH_PAGE_SIZE 2048U              // STM32L4 typical

// Dummy flash area definitions
static const struct flash_area flash_map[] = {
    {
        .fa_id = 0,  // Primary slot
        .fa_device_id = 0,
        .fa_off = FLASH_BASE_ADDRESS + FLASH_BOOTLOADER_SIZE,
        .fa_size = FLASH_APP_SLOT_SIZE,
    },
    {
        .fa_id = 1,  // Secondary slot
        .fa_device_id = 0,
        .fa_off = FLASH_BASE_ADDRESS + FLASH_BOOTLOADER_SIZE + FLASH_APP_SLOT_SIZE,
        .fa_size = FLASH_APP_SLOT_SIZE,
    },
};

#define NUM_FLASH_AREAS (sizeof(flash_map) / sizeof(flash_map[0]))

int flash_area_open(uint8_t id, const struct flash_area **area_outp) {
    for (size_t i = 0; i < NUM_FLASH_AREAS; i++) {
        if (flash_map[i].fa_id == id) {
            *area_outp = &flash_map[i];
            return 0;
        }
    }
    return -1;
}

void flash_area_close(const struct flash_area *fa) {
    (void)fa;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
    if (!fa || !dst || (off + len) > fa->fa_size)
        return -1;

    memcpy(dst, (const void *)(fa->fa_off + off), len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len) {
    if (!fa || !src || (off + len) > fa->fa_size)
        return -1;

    uint32_t addr = fa->fa_off + off;
    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < len; i += 8) {
        uint64_t val64 = 0xFFFFFFFFFFFFFFFF;
        memcpy(&val64, (const uint8_t *)src + i, (len - i) >= 8 ? 8 : (len - i));
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, val64) != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
    if (!fa || (off + len) > fa->fa_size)
        return -1;

    uint32_t start_addr = fa->fa_off + off;
    uint32_t page_error = 0;
    FLASH_EraseInitTypeDef erase_init;

    HAL_FLASH_Unlock();

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Page = (start_addr - FLASH_BASE_ADDRESS) / FLASH_PAGE_SIZE;
    erase_init.NbPages = (len + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    erase_init.Banks = FLASH_BANK_1;

    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;
}

uint32_t flash_area_align(const struct flash_area *area) {
    return 8; // STM32L4 flash writes are 8-byte aligned
}

uint8_t flash_area_erased_val(const struct flash_area *area) {
    return 0xFF;
}

// Extra helpers that mcuboot needs:
int flash_area_get_id(const struct flash_area *fa) {
    return fa->fa_id;
}

uint32_t flash_area_get_off(const struct flash_area *fa) {
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa) {
    return fa->fa_size;
}

// Sector handling (optional - simple single sector per area)
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors) {
    for (size_t i = 0; i < NUM_FLASH_AREAS; i++) {
        if (flash_map[i].fa_id == fa_id) {
            sectors[0].fs_off = 0;
            sectors[0].fs_size = flash_map[i].fa_size;
            *count = 1;
            return 0;
        }
    }
    return -1;
}

uint32_t flash_sector_get_off(const struct flash_sector *sector) {
    return sector->fs_off;
}

uint32_t flash_sector_get_size(const struct flash_sector *sector) {
    return sector->fs_size;
}

// Slot mapping helpers
int flash_area_id_from_multi_image_slot(int image_index, int slot) {
    (void)image_index;
    if (slot == 0) {
        return 0;
    } else if (slot == 1) {
        return 1;
    }
    return -1;
}

int flash_area_id_from_image_slot(int slot) {
    return flash_area_id_from_multi_image_slot(0, slot);
}

// Assert handler (can be empty)
void example_assert_handler(const char *file, int line) {
    (void)file;
    (void)line;
    while (1);
}

uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
    return fa->fa_device_id;
}

const struct flash_sector *flash_area_get_sector(const struct flash_area *fa, uint32_t idx)
{
    static struct flash_sector sector;
    if (idx > 0) {
        return NULL; // only one sector
    }

    sector.fs_off = 0;
    sector.fs_size = fa->fa_size;
    return &sector;
}