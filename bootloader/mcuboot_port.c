#include "bootutil/bootutil_log.h"
#include "mcuboot_config/mcuboot_config.h"
#include "stm32l4a6xx.h"
#include <string.h>

void boot_platform_init(void)
{
    // Nothing needed for basic STM32 platform
}

void boot_platform_quit(void)
{
    // Reset the MCU after booting
    NVIC_SystemReset();
}

void bootutil_log(const char *msg)
{
    // Optional: send log message to UART
    (void)msg;
}
