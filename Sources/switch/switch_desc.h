/*
 * switch_desc.h
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_SWITCH_SWITCH_DESC_H_
#define SOURCES_SWITCH_SWITCH_DESC_H_

#include <stdint.h>
#include <stdbool.h>

// ---- Nintendo Switch Pro Controller Protocol ----

#define SWITCH_PRO_VENDOR_ID     0x057E
#define SWITCH_PRO_PRODUCT_ID    0x2009
#define SWITCH_PRO_ENDPOINT_SIZE 64

// ---- Report IDs ----
// Input (device → host)
#define REPORT_ID_INPUT_30   0x30  // Standard full input report
#define REPORT_ID_INPUT_21   0x21  // Subcommand reply
#define REPORT_ID_INPUT_81   0x81  // 0x80 command reply

// Output (host → device)
#define REPORT_ID_OUTPUT_01  0x01  // UART subcommand
#define REPORT_ID_OUTPUT_10  0x10  // Rumble only
#define REPORT_ID_OUTPUT_80  0x80  // Config/handshake command

// ---- 0x80 Config Sub-commands ----
#define SUBCMD_80_IDENTIFY            0x01
#define SUBCMD_80_HANDSHAKE           0x02
#define SUBCMD_80_BAUD_RATE           0x03
#define SUBCMD_80_DISABLE_USB_TIMEOUT 0x04
#define SUBCMD_80_ENABLE_USB_TIMEOUT  0x05

// ---- 0x01 UART Sub-commands ----
#define SUBCMD_GET_STATE          0x00
#define SUBCMD_BT_PAIR            0x01
#define SUBCMD_DEVICE_INFO        0x02
#define SUBCMD_SET_MODE           0x03
#define SUBCMD_TRIGGER_BUTTONS    0x04
#define SUBCMD_SET_SHIPMENT       0x08
#define SUBCMD_SPI_READ           0x10
#define SUBCMD_SET_NFC_IR_CONFIG  0x21
#define SUBCMD_SET_NFC_IR_STATE   0x22
#define SUBCMD_SET_PLAYER_LIGHTS  0x30
#define SUBCMD_GET_PLAYER_LIGHTS  0x31
#define SUBCMD_SET_HOME_LIGHT     0x38
#define SUBCMD_TOGGLE_IMU         0x40
#define SUBCMD_IMU_SENSITIVITY    0x41
#define SUBCMD_ENABLE_VIBRATION   0x48

// ---- Pro Controller Input Report (0x30) ----
// 3-byte packed 12-bit analogue sticks
typedef struct __attribute__((packed))
{
    uint8_t data[3];
} switch_analog_t;

static inline void analogue_int16_to_le(int16_t in, uint8_t *out)
{
    if(in > 32767)
        in = 32767;
    else if(in < -32768)
        in = -32768;

    out[0] = ((uint16_t)in) & 0xFF;
    out[1] = ((uint16_t)in >> 8) & 0xFF;
}

static inline void switch_analog_set_xy(switch_analog_t *a, uint16_t x, uint16_t y)
{
    a->data[0] = (uint8_t)(x & 0xFF);
    a->data[1] = (uint8_t)(((x >> 8) & 0x0F) | ((y & 0x0F) << 4));
    a->data[2] = (uint8_t)((y >> 4) & 0xFF);
}

static inline void switch_set_accelerometer(uint8_t *imu, size_t sample, int16_t x, int16_t y, int16_t z)
{
    if(sample > 2)
        return;

    analogue_int16_to_le(x, imu + sample + (sample * 12));
    analogue_int16_to_le(y, imu + sample + (sample * 12) + 2);
    analogue_int16_to_le(z, imu + sample + (sample * 12) + 4);
}

static inline void switch_set_gyroscope(uint8_t *imu, size_t sample, int16_t x, int16_t y, int16_t z)
{
    if(sample > 2)
        return;

    analogue_int16_to_le(x, imu + sample + (sample * 12) + 6);
    analogue_int16_to_le(y, imu + sample + (sample * 12) + 6 + 2);
    analogue_int16_to_le(z, imu + sample + (sample * 12) + 6 + 4);
}


typedef struct __attribute__((packed))
{
    uint8_t connection_info : 4;
    uint8_t battery_level   : 4;

    // byte 0: right-side buttons + triggers
    uint8_t btn_y       : 1;
    uint8_t btn_x       : 1;
    uint8_t btn_b       : 1;
    uint8_t btn_a       : 1;
    uint8_t btn_rsr     : 1;  // Right SR (JoyCon)
    uint8_t btn_rsl     : 1;  // Right SL (JoyCon)
    uint8_t btn_r       : 1;
    uint8_t btn_zr      : 1;

    // byte 1: shared buttons
    uint8_t btn_minus   : 1;
    uint8_t btn_plus    : 1;
    uint8_t btn_rstick  : 1;
    uint8_t btn_lstick  : 1;
    uint8_t btn_home    : 1;
    uint8_t btn_capture : 1;
    uint8_t _pad0       : 1;
    uint8_t charging    : 1;

    // byte 2: left-side buttons + triggers
    uint8_t dpad_down   : 1;
    uint8_t dpad_up     : 1;
    uint8_t dpad_right  : 1;
    uint8_t dpad_left   : 1;
    uint8_t btn_lsr     : 1;  // Left SR (JoyCon)
    uint8_t btn_lsl     : 1;  // Left SL (JoyCon)
    uint8_t btn_l       : 1;
    uint8_t btn_zl      : 1;

    switch_analog_t left_stick;
    switch_analog_t right_stick;
} switch_input_report_t;

// Full 0x30 input report (64 bytes)
typedef struct __attribute__((packed))
{
    uint8_t report_id;              // 0x30
    uint8_t timestamp;              // incrementing counter
    switch_input_report_t input;
    uint8_t rumble_report;          // vibro-motor input report
    uint8_t imu_data[36];           // 3 IMU samples (empty)
    uint8_t padding[15];            // zero padding to 64 bytes
} switch_pro_report_t;

// ---- Protocol state ----
typedef struct
{
    bool     handshake_done;        // 0x80 handshake complete
    bool     reports_enabled;       // Switch asked for input reports
    bool     imu_enabled;           // Enable sending of the IMU data
    uint8_t  imu_data_input[36];    // Input IMU samples, gets copied into report when imu is enabled
    uint8_t  report_counter;        // incrementing time stamp for 0x30
    uint8_t  report_buf[64];        // scratch buffer for responses
    uint8_t  player_id;             // player LED state
} switch_pro_state_t;

#endif /* SOURCES_SWITCH_SWITCH_DESC_H_ */
