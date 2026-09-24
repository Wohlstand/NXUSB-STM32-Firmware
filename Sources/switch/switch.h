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

#ifndef SOURCES_SWITCH_SWITCH_H_
#define SOURCES_SWITCH_SWITCH_H_

#include <stdbool.h>

extern void switch_pro_handle_output(const uint8_t *data, uint16_t len);

extern void switch_init_input_state(void);

extern void switch_input_set_defaults(void);

extern void switch_input_state_reset(void);

extern void switch_query_player(void);
extern void switch_query_imu(void);
extern void switch_query_vibro(void);

extern void switch_query_config(void);

extern bool switch_pro_send_queued(void);

enum NXCommand
{
    CMD_None = 0,
    CMD_ButtonsUpdate = 1,  // 14
    CMD_Tilt = 2,           // 14
    CMD_Ping = 3,           // 1
    CMD_Reset = 4,          // 1
    CMD_QueryPlayer = 5,    // 1
    CMD_QueryIMU = 6,       // 1
    CMD_QueryVibro = 7,     // 1
    CMD_QueryConfig = 8,     // 1

    CMD_END
};

struct __attribute__((packed)) NXSendCmd
{
    // Command of @NXCommand type
    uint8_t cmd;

    union Data
    {
        struct Buttons
        {
            uint32_t buttons;
            uint16_t stick_l[2];
            uint16_t stick_r[2];
        } state;

        // Accelerometer / Gyroscope X, Y, Z
        struct Tilt
        {
            int16_t accel[3];
            int16_t gyro[3];
        } tilt;

        struct Tail
        {
            uint8_t tail[12];
        } tail;
    } data;

    uint8_t tail;
};

extern bool switch_pro_active(void);

extern void switch_receive(const struct NXSendCmd *in);

extern void switch_update_state(void);

#endif /* SOURCES_SWITCH_SWITCH_H_ */
