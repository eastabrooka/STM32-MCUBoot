#include <stdint.h>
#include "bootutil/bootutil.h"
#include "stm32l4a6xx.h"
#include "core_cm4.h"

#include <stdio.h>
#include "serial_interface.h"


extern void SystemInit(void);
void MX_LPUART1_UART_Init(void);

int main(void)
{
    struct boot_rsp rsp;

    // Basic hardware init (optional but recommended for STM32)
    SystemInit();

    MX_LPUART1_UART_Init();
    uint8_t c = 'A';
    HAL_UART_Transmit(&hlpuart1, &c, 1, HAL_MAX_DELAY);
    printf("Hello world!\r\n");


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


void MX_LPUART1_UART_Init(void)
{
    // 1) Enable the LPUART1 clock
    __HAL_RCC_LPUART1_CLK_ENABLE();

    // 2) Configure the GPIOs (you can leave your MX_GPIO_Init as-is,
    //    or move it here if you prefer)
    MX_GPIO_Init();

    // 3) Init the UART
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&hlpuart1);

}
