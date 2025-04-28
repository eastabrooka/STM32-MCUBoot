#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdio.h>         // for printf

// The UART instance you use for console
extern UART_HandleTypeDef hlpuart1;

// GPIO + UART init (from serial_interface.c)
void MX_GPIO_Init(void);
void MX_LPUART1_UART_Init(void);

// RX ring buffer
int  Serial_ReadChar(void);

// printf() / putchar() redirect
int  __io_putchar(int ch);

#endif // SERIAL_INTERFACE_H
