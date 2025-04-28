/* Run the boot image. */

#include <string.h>

#include "flash_map_backend/flash_map_backend.h"
#include "os/os_malloc.h"
#include "sysflash/sysflash.h"
#include "mcuboot_config/mcuboot_logging.h"
#include "mcuboot_config/mcuboot_assert.h"



#define BOOTLOADER_START_ADDRESS 0x0
#define BOOTLOADER_SIZE (32 * 1024)
#define APPLICATION_PRIMARY_START_ADDRESS (32 * 1024)
#define APPLICATION_SECONDARY_START_ADDRESS (APPLICATION_PRIMARY_START_ADDRESS + APPLICATION_SIZE)
#define APPLICATION_SIZE (128 * 1024)


int flash_area_open(uint8_t id, const struct flash_area **area_outp) {
  return -1;
}

void flash_area_close(const struct flash_area *area) {
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst,
                    uint32_t len) {
  return -1;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src,
                     uint32_t len) {
  return -1;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
  return -1;
}

size_t flash_area_align(const struct flash_area *area) {
  return 1;
}

uint8_t flash_area_erased_val(const struct flash_area *area) {
  return 0xff;
}

int flash_area_get_sectors(int fa_id, uint32_t *count,
                           struct flash_sector *sectors) {
  return -1;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot) {
  return -1;
}

int flash_area_id_from_image_slot(int slot) {
  return flash_area_id_from_multi_image_slot(0, slot);
}


void example_assert_handler(const char *file, int line) {
}


