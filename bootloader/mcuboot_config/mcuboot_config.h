#pragma once

// Enable overwrite-only mode (no swap)
#define MCUBOOT_OVERWRITE_ONLY 1

// Only one image slot (image 0)
#define MCUBOOT_IMAGE_NUMBER 1

// No signature checking (no RSA, no ECDSA, no crypto)
#define MCUBOOT_SIGN_RSA       0
#define MCUBOOT_SIGN_EC256     0
#define MCUBOOT_SIGN_ED25519   0

// No encryption
#define MCUBOOT_ENCRYPT_RSA    0
#define MCUBOOT_ENCRYPT_EC256  0

// No boot trailer validation (optional but cleaner)
#define MCUBOOT_VALIDATE_PRIMARY_SLOT 0

// Disable upgrade (because no OTA in this basic setup)
#define MCUBOOT_DIRECT_XIP 0

// Disable logging
#define MCUBOOT_HAVE_LOGGING 0
#define MCUBOOT_LOG_LEVEL 0
