/*
 * switch.h
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_SWITCH_SWITCH_H_
#define SOURCES_SWITCH_SWITCH_H_

#include <stdbool.h>

extern void switch_pro_handle_output(const uint8_t *data, uint16_t len);

extern bool switch_pro_send_queued(void);

enum NXCommand
{
    CMD_None = 0,
    CMD_ButtonsUpdate = 1,
    CMD_Tilt = 2,
    CMD_Ping = 3,
    CMD_Reset = 4,
};

struct __attribute__((packed)) NXSendCmd
{
    // Command of @NXCommand type
    uint8_t cmd;

    union
    {
        struct Buttons
        {
            uint32_t buttons;
            uint16_t stick_l[2];
            uint16_t stick_r[2];
        } state;

        // Accelerometer / Gyroscope X, Y, Z, 3 samples
        struct Tilt
        {
            uint16_t accel[3];
            uint16_t gyro[3];
        } tilt;

        struct Padding
        {
            uint8_t junk[12];
        } pad;
    } data;

    uint8_t tail;
};

extern void switch_receive(const struct NXSendCmd *in);

extern void switch_update_state(void);

#endif /* SOURCES_SWITCH_SWITCH_H_ */
