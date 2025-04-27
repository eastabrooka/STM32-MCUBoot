#include "stm32l4a6xx.h"

uint32_t SystemCoreClock = 4000000; // 4 MHz default after reset

void SystemInit(void)
{
    // Reset all RCC settings (if you want safe startup)
    RCC->CR |= RCC_CR_MSION;         // Enable MSI
    RCC->CFGR = 0;                   // Reset clock config
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_PLLON); // Turn off HSI, HSE, PLL
    RCC->PLLCFGR = 0x00001000;        // Reset PLL config
    RCC->CIER = 0;                    // Disable all RCC interrupts

    // Set vector table base address if needed (optional)
    SCB->VTOR = FLASH_BASE | 0x0;
}

void SystemCoreClockUpdate(void)
{
    // We don't need dynamic clock tracking for MCUboot
    SystemCoreClock = 4000000; // Always assume MSI 4 MHz
}
