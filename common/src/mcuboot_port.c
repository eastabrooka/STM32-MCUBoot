#include <string.h>

#include "flash_map_backend/flash_map_backend.h"
#include "os/os_malloc.h"
#include "sysflash/sysflash.h"

#include "logging.h"
#include "internal_flash.h"

#include "mcuboot_config/mcuboot_logging.h"
#include "mcuboot_config/mcuboot_assert.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash.h"

#define BOOTLOADER_START_ADDRESS    0x08000000
#define BOOTLOADER_SIZE             (96 * 1024)        /* was 128 KB */

#define APPLICATION_PRIMARY_START_ADDRESS  0x08018000    /* was 0x08020000 */
#define APPLICATION_SIZE                  (128 * 1024)

#define APPLICATION_SECONDARY_START_ADDRESS \
    (APPLICATION_PRIMARY_START_ADDRESS + APPLICATION_SIZE)  /* now 0x08038000 */
#define SCRATCH_START_ADDRESS \
    (APPLICATION_SECONDARY_START_ADDRESS + APPLICATION_SIZE) /* now 0x08058000 */
#define SCRATCH_SIZE                      (16 * 1024)
#define FLASH_SECTOR_SIZE 2048U
#define FLASH_BANK2_BASE 0x08080000U

static const struct flash_area bootloader = {
    .fa_id = FLASH_AREA_BOOTLOADER,
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = BOOTLOADER_START_ADDRESS,
    .fa_size = BOOTLOADER_SIZE,
};

static const struct flash_area primary_img0 = {
    .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = APPLICATION_PRIMARY_START_ADDRESS,
    .fa_size = APPLICATION_SIZE,
};

static const struct flash_area secondary_img0 = {
    .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = APPLICATION_SECONDARY_START_ADDRESS,
    .fa_size = APPLICATION_SIZE,
};

static const struct flash_area scratch = {
    .fa_id = FLASH_AREA_IMAGE_SCRATCH,
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = SCRATCH_START_ADDRESS,
    .fa_size = SCRATCH_SIZE,
};

static const struct flash_area *s_flash_areas[] = {
    &bootloader, &primary_img0, &secondary_img0, &scratch,
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static const struct flash_area *prv_lookup_flash_area(uint8_t id) {
    for (size_t i = 0; i < ARRAY_SIZE(s_flash_areas); i++) {
        if (s_flash_areas[i]->fa_id == id) {
            return s_flash_areas[i];
        }
    }
    return NULL;
}

int flash_area_open(uint8_t id, const struct flash_area **area_outp) {
    const struct flash_area *area = prv_lookup_flash_area(id);
    *area_outp = area;
    return area != NULL ? 0 : -1;
}

void flash_area_close(const struct flash_area *fa) {
    (void)fa;
}

uint32_t flash_area_align(const struct flash_area *area) {
    return 8;
}

uint8_t flash_area_erased_val(const struct flash_area *area) {
    return 0xFF;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors) {
    const struct flash_area *fa = prv_lookup_flash_area(fa_id);
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    uint32_t total = 0;
    for (size_t off = 0; off < fa->fa_size; off += FLASH_SECTOR_SIZE) {
        sectors[total].fs_off = off;
        sectors[total].fs_size = FLASH_SECTOR_SIZE;
        total++;
    }

    *count = total;
    return 0;
}

// 🔧 Flash controller cleanup
void flash_controller_prepare(void) {
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    printf("[FLASH PREP] CR=0x%08lX SR=0x%08lX\n", FLASH->CR, FLASH->SR);
}

void example_internal_flash_write(uint32_t addr, const void *buf, size_t length) {
    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < length; i += 8) {
        uint64_t word = 0xFFFFFFFFFFFFFFFFULL;
        memcpy(&word, (const uint8_t *)buf + i, (length - i >= 8) ? 8 : (length - i));
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, word) != HAL_OK) {
            while (1);
        }
    }
    HAL_FLASH_Lock();
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) return -1;
    if ((off + len) > fa->fa_size) return -1;

    memcpy(dst, (void *)(fa->fa_off + off), len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len) {
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) return -1;
    if ((off + len) > fa->fa_size) return -1;

    uint32_t addr = fa->fa_off + off;
    example_internal_flash_write(addr, src, len);

#if VALIDATE_PROGRAM_OP
    if (memcmp((void *)addr, src, len) != 0) {
        MCUBOOT_LOG_ERR("%s: Program validation failed", __func__);
        assert(0);
    }
#endif

    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) return -1;
    if ((off % FLASH_SECTOR_SIZE) || (len % FLASH_SECTOR_SIZE)) return -1;

    uint32_t addr = fa->fa_off + off;
    uint32_t end_addr = addr + len;

    flash_controller_prepare();  // ← ensure controller is clean

    while (addr < end_addr) {
        FLASH_EraseInitTypeDef erase_init = {0};
        uint32_t page_error = 0;
        uint32_t page;

        if (addr < FLASH_BANK2_BASE) {
            erase_init.Banks = FLASH_BANK_1;
            page = (addr - 0x08000000) / FLASH_SECTOR_SIZE;
        } else {
            erase_init.Banks = FLASH_BANK_2;
            page = (addr - 0x08080000) / FLASH_SECTOR_SIZE;
        }

        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.Page = page;
        erase_init.NbPages = 1;

        printf("[ERASE DEBUG] Bank=%lu Page=%lu Addr=0x%08lX\n",
               (unsigned long)erase_init.Banks,
               (unsigned long)erase_init.Page,
               (unsigned long)addr);

        HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &page_error);

        printf("[FLASH AFTER ERASE TRY] CR=0x%08lX SR=0x%08lX\n", FLASH->CR, FLASH->SR);

        if (status != HAL_OK) {
            printf("[ERASE ERROR] Status=%d PageError=0x%08lX at Addr=0x%08lX\n",
                   status, page_error, (unsigned long)addr);
            HAL_FLASH_Lock();
            assert(0);
        } else {
            printf("[ERASE OK] Page erased at Addr=0x%08lX\n", (unsigned long)addr);
        }

        addr += FLASH_SECTOR_SIZE;
    }

    HAL_FLASH_Lock();
    return 0;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa) {
    return fa->fa_device_id;
}

uint32_t flash_area_get_off(const struct flash_area *fa) {
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa) {
    return fa->fa_size;
}

uint8_t flash_area_get_id(const struct flash_area *fa) {
    return fa->fa_id;
}

int flash_area_get_sector(uint32_t off, struct flash_sector *sector) {
    sector->fs_off = off;
    sector->fs_size = FLASH_SECTOR_SIZE;
    return 0;
}

uint32_t flash_sector_get_off(const struct flash_sector *sector) {
    return sector->fs_off;
}

uint32_t flash_sector_get_size(const struct flash_sector *sector) {
    return sector->fs_size;
}

void example_assert_handler(const char *file, int line) {
    EXAMPLE_LOG("ASSERT: File: %s Line: %d", file, line);
    __builtin_trap();
}

int flash_area_id_from_multi_image_slot(int image_index, int slot) {
    (void)image_index;
    switch (slot) {
        case 0: return FLASH_AREA_IMAGE_PRIMARY(0);
        case 1: return FLASH_AREA_IMAGE_SECONDARY(0);
        default:
            MCUBOOT_LOG_ERR("Invalid slot: %d", slot);
            return -1;
    }
}

int flash_area_id_from_image_slot(int slot) {
    return flash_area_id_from_multi_image_slot(0, slot);
}
