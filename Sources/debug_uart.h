#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "stm32f1xx_hal.h"

// Initialize after MX_USART1_UART_Init() has been called
extern void debug_init(UART_HandleTypeDef *huart);

// Print a string (blocking, for debug only)
extern void debug_print(const char *str);

// Print a hex byte
extern void debug_hex8(uint8_t val);

// Print a hex word
extern void debug_hex16(uint16_t val);

// Print a hex dword
extern void debug_hex32(uint32_t val);

// Print string + newline
extern void debug_println(const char *str);

// Print a buffer as hex dump (up to 64 bytes)
extern void debug_dump(const char *label, const uint8_t *data, uint8_t len);

#endif
