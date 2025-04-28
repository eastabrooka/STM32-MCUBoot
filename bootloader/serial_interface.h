#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include <stdint.h>

/// Call once at startup, e.g. Serial_Init(115200);
void Serial_Init(uint32_t baudrate);

/// Reads one byte from the RX ring buffer, or –1 if empty
int  Serial_ReadChar(void);

/// Redirect printf (or putchar) to UART
int  __io_putchar(int ch);

#endif // SERIAL_INTERFACE_H
