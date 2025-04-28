#include <string.h>

#include "flash_map_backend/flash_map_backend.h"
#include "os/os_malloc.h"
#include "sysflash/sysflash.h"

#include "logging.h"
#include "internal_flash.h"

#include "mcuboot_config/mcuboot_logging.h"
#include "mcuboot_config/mcuboot_assert.h"

#include "stm32l4xx_hal.h" // for FLASH functions

//
// Flash layout
//

#define BOOTLOADER_START_ADDRESS 0x08000000
#define BOOTLOADER_SIZE (128 * 1024) // 128KB bootloader flash region

#define APPLICATION_PRIMARY_START_ADDRESS 0x08020000
#define APPLICATION_SIZE (128 * 1024) // 128KB for primary image

#define APPLICATION_SECONDARY_START_ADDRESS (APPLICATION_PRIMARY_START_ADDRESS + APPLICATION_SIZE)

//
// Flash sectors
//

#define FLASH_SECTOR_SIZE 2048U // 2KB sectors for STM32L4
//#define FLASH_SECTOR_SIZE 4096U // 4KB sectors for STM32L4A6

//
// Flash areas
//

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

static const struct flash_area *s_flash_areas[] = {
    &bootloader,
    &primary_img0,
    &secondary_img0,
};

//
// Flash area helpers
//

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
    // no-op for STM32 internal flash
}

uint32_t flash_area_align(const struct flash_area *area) {
    return 8; // STM32L4 requires 8-byte alignment
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

//
// Internal flash low-level write/erase
//

void example_internal_flash_write(uint32_t addr, const void *buf, size_t length) {
  HAL_FLASH_Unlock();

  for (uint32_t i = 0; i < length; i += 8) {
      uint64_t word = 0xFFFFFFFFFFFFFFFFULL;
      memcpy(&word, (const uint8_t *)buf + i, (length - i >= 8) ? 8 : (length - i));
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, word) != HAL_OK) {
          // handle error
          while (1);
      }
  }

  HAL_FLASH_Lock();
}

void example_internal_flash_erase_sector(uint32_t addr) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase_init = {0};
    uint32_t page_error = 0;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Page = (addr - 0x08000000) / FLASH_SECTOR_SIZE;
    erase_init.NbPages = 1;
    erase_init.Banks = FLASH_BANK_1;

    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
        // handle error
        while (1);
    }

    HAL_FLASH_Lock();
}

//
// Flash area read/write/erase
//

#define VALIDATE_PROGRAM_OP 1

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    uint32_t end_offset = off + len;
    if (end_offset > fa->fa_size) {
        MCUBOOT_LOG_ERR("%s: Out of bounds (0x%x vs 0x%x)", __func__,
                        (unsigned int)end_offset, (unsigned int)fa->fa_size);
        return -1;
    }

    void *src_addr = (void *)(fa->fa_off + off);
    memcpy(dst, src_addr, len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len) {
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    uint32_t end_offset = off + len;
    if (end_offset > fa->fa_size) {
        MCUBOOT_LOG_ERR("%s: Out of bounds (0x%x vs 0x%x)", __func__,
                        (unsigned int)end_offset, (unsigned int)fa->fa_size);
        return -1;
    }

    uint32_t addr = fa->fa_off + off;
    MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (unsigned int)addr, (int)len);

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
    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    if ((off % FLASH_SECTOR_SIZE) != 0 || (len % FLASH_SECTOR_SIZE) != 0) {
        MCUBOOT_LOG_ERR("%s: Not sector aligned (0x%x, 0x%x)", __func__,
                        (unsigned int)off, (unsigned int)len);
        return -1;
    }

    uint32_t addr = fa->fa_off + off;
    MCUBOOT_LOG_DBG("%s: Erase Addr: 0x%08x Length: %d", __func__,
                    (unsigned int)addr, (int)len);

    for (uint32_t i = 0; i < len; i += FLASH_SECTOR_SIZE) {
        example_internal_flash_erase_sector(addr + i);
    }

#if VALIDATE_PROGRAM_OP
    for (uint32_t i = 0; i < len; i++) {
        if (*(volatile uint8_t *)(addr + i) != 0xFF) {
            MCUBOOT_LOG_ERR("%s: Erase verify failed at 0x%x", __func__,
                            (unsigned int)(addr + i));
            assert(0);
        }
    }
#endif

    return 0;
}

//
// Flash sector and area helpers needed by bootutil
//

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

//
// Assert hook
//

void example_assert_handler(const char *file, int line) {
    EXAMPLE_LOG("ASSERT: File: %s Line: %d", file, line);
    __builtin_trap();
}


int flash_area_id_from_multi_image_slot(int image_index, int slot) {
  (void)image_index; // ignore because we only support image 0

  switch (slot) {
  case 0:
      return FLASH_AREA_IMAGE_PRIMARY(0);
  case 1:
      return FLASH_AREA_IMAGE_SECONDARY(0);
  default:
      MCUBOOT_LOG_ERR("Invalid slot: %d", slot);
      return -1;
  }
}


int flash_area_id_from_image_slot(int slot) {
    return flash_area_id_from_multi_image_slot(0, slot);
}