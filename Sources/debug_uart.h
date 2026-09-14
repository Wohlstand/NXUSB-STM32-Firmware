/*
 * UART to USB HID proxy for the Nintendo Switch
 *
 * Copyright (c) 2026-2026 Vitaly Novichkov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "stm32f1xx_hal.h"

// Initialize after MX_USART1_UART_Init() has been called
extern void debug_init(UART_HandleTypeDef *huart);

extern void debug_insert(const char *str);

extern void debug_print_begin(void);
extern void debug_print_end(void);

extern void debug_imu_state(uint8_t enabled);
extern void debug_vibro(uint8_t enabled);
extern void debug_palyer(uint8_t player);

extern void debug_config(const uint8_t *samples, uint8_t size);

extern void debug_rumble(const uint8_t *samples, uint8_t size);
extern void debug_home_light(const uint8_t *samples, uint8_t size);
extern void debug_imu_sens(const uint8_t *samples, uint8_t size);
extern void debug_imu_reg_write(const uint8_t *samples, uint8_t size);

// Print a hex byte
extern void debug_hex8(uint8_t val);

// Print a hex word
extern void debug_hex16(uint16_t val);

// Print a hex dword
extern void debug_hex32(uint32_t val);

// Print string + newline
extern void debug_println(const char *str);

extern void debug_printf(const char *format, ...);

// Print a buffer as hex dump (up to 64 bytes)
extern void debug_dump(const char *label, const uint8_t *data, uint8_t len);

#endif
