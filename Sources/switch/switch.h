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

extern void switch_receive(const struct NXSendCmd *in);

extern void switch_update_state(void);

#endif /* SOURCES_SWITCH_SWITCH_H_ */
