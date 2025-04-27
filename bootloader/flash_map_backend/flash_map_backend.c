#include "flash_map_backend/flash_map_backend.h"
#include "sysflash.h"
#include <string.h>

extern uint8_t _flash_primary_start;  // Symbols from linker
extern uint8_t _flash_secondary_start;
extern uint8_t _flash_bootloader_start;

#define SLOT0_START 0x08008000  // Primary Slot Address (after bootloader)
#define SLOT1_START 0x08028000  // Secondary Slot Address
#define SLOT_SIZE   0x20000     // 128 KB per slot

struct flash_area {
    uint8_t  fa_id;
    uint8_t  fa_device_id;
    uint16_t pad;
    uint32_t fa_off;    // Offset within device
    uint32_t fa_size;   // Length of area
};

static const struct flash_area flash_areas[] = {
    {
        .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
        .fa_device_id = FLASH_DEVICE_ID_INTERNAL,
        .fa_off = SLOT0_START,
        .fa_size = SLOT_SIZE,
    },
    {
        .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
        .fa_device_id = FLASH_DEVICE_ID_INTERNAL,
        .fa_off = SLOT1_START,
        .fa_size = SLOT_SIZE,
    },
};

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
    for (int i = 0; i < sizeof(flash_areas)/sizeof(flash_areas[0]); i++) {
        if (flash_areas[i].fa_id == id) {
            *fa = &flash_areas[i];
            return 0;
        }
    }
    return -1; // Not found
}

void flash_area_close(const struct flash_area *fa)
{
    (void)fa;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
    if (off + len > fa->fa_size) {
        return -1;
    }
    memcpy(dst, (void *)(fa->fa_off + off), len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len)
{
    if (off + len > fa->fa_size) {
        return -1;
    }

    // STM32 internal flash write (simple blocking write)
    HAL_FLASH_Unlock();

    uint32_t addr = fa->fa_off + off;
    const uint8_t *src_bytes = (const uint8_t *)src;

    for (uint32_t i = 0; i < len; i += 1) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, src_bytes[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
    if (off + len > fa->fa_size) {
        return -1;
    }

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    uint32_t addr = fa->fa_off + off;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Page = (addr - 0x08000000) / FLASH_PAGE_SIZE;
    erase_init.NbPages = (len + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    erase_init.Banks = FLASH_BANK_1;

    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;
}

size_t flash_area_align(const struct flash_area *fa)
{
    (void)fa;
    return 8; // STM32 usually has 8-byte alignment requirement
}

uint8_t flash_area_erased_val(const struct flash_area *fa)
{
    (void)fa;
    return 0xFF; // Erased flash is all 0xFF
}
