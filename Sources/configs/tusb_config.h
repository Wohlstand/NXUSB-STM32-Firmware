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

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// ---- MCU selection ----
#define CFG_TUSB_MCU   OPT_MCU_STM32F1

// ---- Root hub port 0 = Device mode, Full Speed ----
#define CFG_TUSB_RHPORT0_MODE  OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED

#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_FULL_SPEED
#endif

// ---- RTOS (none = bare metal) ----
#define CFG_TUSB_OS    OPT_OS_NONE

// ---- Debug ----
//#define CFG_TUSB_DEBUG 1
//#define CFG_TUSB_DEBUG_PRINTF mine_debug_print

// ---- Endpoint0 max packet size (must be 64 for Switch) ----
#define CFG_TUD_ENDPOINT0_SIZE  64

// ---- Device stack ----
#define CFG_TUD_ENABLED  1
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

// ---- HID: 1 interface (Pro Controller) ----
#define CFG_TUD_HID      1
#define CFG_TUD_HID_EP_BUFSIZE  64

// Disable unused classes
#define CFG_TUD_CDC      0
#define CFG_TUD_MSC      0
#define CFG_TUD_MIDI     0
#define CFG_TUD_VENDOR   0

#ifdef __cplusplus
}
#endif
#endif
