#include <stdint.h>
#include "bootutil/bootutil.h"
#include "stm32l4a6xx.h"
#include "core_cm4.h"

extern void SystemInit(void);

int main(void)
{
    struct boot_rsp rsp;

    // Basic hardware init (optional but recommended for STM32)
    SystemInit();

    // Try to find a valid image to boot
    int rc = boot_go(&rsp);
    if (rc != 0) {
        // Failed to find valid image, trap here
        while (1) { }
    }

    // Found an image
    uint32_t app_hdr_addr = rsp.br_image_off;
    uint32_t app_vector_table = app_hdr_addr + 0x100; // MCUboot header is 0x100 bytes
    uint32_t sp = *(uint32_t *)(app_vector_table + 0x0);
    uint32_t reset = *(uint32_t *)(app_vector_table + 0x4);

    // Disable interrupts before jumping
    __disable_irq();

    // Set vector table offset to application
    SCB->VTOR = app_vector_table;

    // Set MSP (Main Stack Pointer) to app's initial value
    __set_MSP(sp);

    // Jump to application's Reset_Handler
    ((void (*)(void))reset)();

    // Should never reach here
    while (1) { }
}
