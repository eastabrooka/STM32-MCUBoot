#pragma once


#define MCUBOOT_USE_TINYCRYPT

// Area IDs for images
#define FLASH_AREA_IMAGE_PRIMARY(x)    (1)
#define FLASH_AREA_IMAGE_SECONDARY(x)  (2)

// Tell MCUBoot what behavior you want
#define MCUBOOT_OVERWRITE_ONLY 1
#define MCUBOOT_PRIMARY_ONLY 1
#define MCUBOOT_MAX_IMG_SECTORS   128


// Disable logging if not needed
#define MCUBOOT_HAVE_LOGGING 0

// Fake logging macros if needed
#define MCUBOOT_LOG_ERR(...) 
#define MCUBOOT_LOG_WRN(...) 
#define MCUBOOT_LOG_INF(...) 
#define MCUBOOT_LOG_DBG(...)


#define MCUBOOT_WATCHDOG_FEED()    do {} while (0)

