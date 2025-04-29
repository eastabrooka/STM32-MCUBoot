#pragma once

// === General options ===
#define MCUBOOT_USE_FLASH_AREA_GET_SECTORS 1
#define MCUBOOT_BOOTSTRAP                  1

// === Signature method ===
#define MCUBOOT_SIGN_RSA                   1
#define MCUBOOT_SIGN_RSA_LEN              2048
#define MCUBOOT_USE_MBED_TLS              1

// === Upgrade method ===
#define MCUBOOT_SWAP_USING_MOVE           1

// === Validation ===
#define MCUBOOT_VALIDATE_PRIMARY_SLOT     0
#define MCUBOOT_IMAGE_NUMBER              1
#define MCUBOOT_MAX_IMG_SECTORS           256

// === Logging and Assert ===
#define MCUBOOT_HAVE_LOGGING              1
#define MCUBOOT_HAVE_ASSERT_H             1

// === Watchdog ===
#define MCUBOOT_WATCHDOG_FEED()
