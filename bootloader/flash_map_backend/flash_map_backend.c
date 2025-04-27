#include "flash_map_backend.h"
#include "stm32l4a6xx.h"
#include <string.h>
#include <stddef.h>

extern uint8_t _flash_primary_start;  // Symbols from linker
extern uint8_t _flash_secondary_start;
extern uint8_t _flash_bootloader_start;

#define SLOT0_START 0x08008000  // Primary Slot Address (after bootloader)
#define SLOT1_START 0x08028000  // Secondary Slot Address
#define SLOT_SIZE   0x20000     // 128 KB per slot
#define FLASH_PAGE_SIZE 2048    // STM32L4 page size in bytes



static const struct flash_area flash_areas[] = {
    {
        .fa_id = 1,  // Primary slot
        .fa_device_id = 0,
        .fa_off = SLOT0_START,
        .fa_size = SLOT_SIZE,
    },
    {
        .fa_id = 2,  // Secondary slot
        .fa_device_id = 0,
        .fa_off = SLOT1_START,
        .fa_size = SLOT_SIZE,
    },
};

static void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

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

    flash_unlock();

    uint32_t addr = fa->fa_off + off;
    const uint8_t *src_bytes = (const uint8_t *)src;

    for (uint32_t i = 0; i < len; i++) {
        while (FLASH->SR & FLASH_SR_BSY) {
        }
        FLASH->CR |= FLASH_CR_PG;
        *(volatile uint8_t *)(addr + i) = src_bytes[i];
        while (FLASH->SR & FLASH_SR_BSY) {
        }
        FLASH->CR &= ~FLASH_CR_PG;
    }

    flash_lock();
    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
    if (off + len > fa->fa_size) {
        return -1;
    }

    flash_unlock();

    uint32_t addr = fa->fa_off + off;
    uint32_t page = (addr - 0x08000000) / FLASH_PAGE_SIZE;
    uint32_t num_pages = (len + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    for (uint32_t i = 0; i < num_pages; i++) {
        FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PER | FLASH_CR_MER1 | FLASH_CR_MER2);
        FLASH->CR |= FLASH_CR_PER;
        FLASH->CR &= ~FLASH_CR_PNB;
        FLASH->CR |= (page + i) << FLASH_CR_PNB_Pos; // Set Page Number
        FLASH->CR |= FLASH_CR_STRT;

        while (FLASH->SR & FLASH_SR_BSY) {
        }

        // Check for errors
        if (FLASH->SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR | FLASH_SR_PGSERR)) {
            flash_lock();
            return -1;
        }
    }

    FLASH->CR &= ~FLASH_CR_PER;
    flash_lock();
    return 0;
}

size_t flash_area_align(const struct flash_area *fa)
{
    (void)fa;
    return 8; // STM32L4 typically has 8-byte alignment
}

uint8_t flash_area_erased_val(const struct flash_area *fa)
{
    (void)fa;
    return 0xFF; // Erased flash reads 0xFF
}
