#include "serial_interface.h"
#include "stm32l4xx.h"    // CMSIS device header

#define RX_BUFFER_SIZE 128

static volatile uint8_t  rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_head, rx_tail;

/// Initialize PG7=TX, PG8=RX and LPUART1 at given baud (no HAL)
void Serial_Init(uint32_t baudrate)
{
    // 1) Enable clocks
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOGEN;
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

    // 2) Configure PG7/PG8 to AF8
    GPIOG->MODER   &= ~((3U<<14)|(3U<<16));
    GPIOG->MODER   |=  (2U<<14)|(2U<<16);  // AF mode
    GPIOG->AFR[0]  &= ~((0xFU<<28)|(0xFU<<32));
    GPIOG->AFR[0]  |=  (8U<<28)|(8U<<32);
    GPIOG->OSPEEDR |=  (2U<<14)|(2U<<16);  // medium speed

    // 3) Disable LPUART while configuring
    LPUART1->CR1 &= ~USART_CR1_UE;

    // 4) Set baud: BRR = Fck / baud
    //    assume PCLK = 80 MHz; adjust if different
    uint32_t clk = 80000000;
    LPUART1->BRR = (clk + baudrate/2) / baudrate;

    // 5) 8N1, TX & RX enable
    LPUART1->CR1 = USART_CR1_TE | USART_CR1_RE;

    // 6) No parity, 1 stop bit (CR2/CR3 default = 0)

    // 7) Enable RX interrupt
    LPUART1->CR1 |= USART_CR1_RXNEIE;

    // 8) Enable UART
    LPUART1->CR1 |= USART_CR1_UE;

    // 9) NVIC
    NVIC_EnableIRQ(LPUART1_IRQn);
}

/// IRQ handler: read one byte, push into ring buffer
void LPUART1_IRQHandler(void)
{
    if (LPUART1->ISR & USART_ISR_RXNE) {
        uint8_t b = (uint8_t)(LPUART1->RDR & 0xFF);
        uint16_t next = (rx_head + 1) % RX_BUFFER_SIZE;
        if (next != rx_tail) {
            rx_buffer[rx_head] = b;
            rx_head = next;
        }
        // RXNE cleared by reading RDR
    }
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
