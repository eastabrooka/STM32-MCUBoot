#include "serial_interface.h"
#include "stm32l4xx.h"    // CMSIS device header

#define RX_BUFFER_SIZE 128

static volatile uint8_t  rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_head, rx_tail;


void MX_LPUART1_UART_Init(void)
{
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

/// Read one char or –1 if none
int Serial_ReadChar(void)
{
    if (rx_head == rx_tail) return -1;
    uint8_t c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return c;
}

/// Redirect printf() / putchar() to UART
int __io_putchar(int ch)
{
    // wait TXE
    while ((LPUART1->ISR & USART_ISR_TXE) == 0);
    LPUART1->TDR = (uint8_t)ch;
    return ch;
}


// tell the linker to use this instead of semihosting
int _write(int file, char *ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        __io_putchar(ptr[i]);
    }
    return len;
}

