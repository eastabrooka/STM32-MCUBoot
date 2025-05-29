#include <stm32l4xx_hal.h>
#include <stdio.h>
#include "serial_interface.h"

UART_HandleTypeDef hlpuart1;

#define RX_BUFFER_SIZE 128

// Define which UART to use for printf 
#define PRINTF_UART hlpuart1 
 


volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_head = 0;
volatile uint16_t rx_tail = 0;

static uint8_t rx_byte;  // temp storage for each received byte

void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}


void LPUART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&hlpuart1);
}


// Redirect printf output to UART 
int __io_putchar(int ch) 
{ 
    HAL_UART_Transmit(&PRINTF_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY); 
    return ch; 
} 