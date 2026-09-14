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
#define REPORT_ID_INPUT_21   0x21  // Sub-command reply
#define REPORT_ID_INPUT_81   0x81  // 0x80 command reply

// Output (host → device)
#define REPORT_ID_OUTPUT_01  0x01  // UART sub-command
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
#define SUBCMD_GET_PAGELIST_STATE 0x05
#define SUBCMD_SET_HCI_STATE      0x06
#define SUBCMD_RESET_PAIR_INFO    0x07
#define SUBCMD_SET_SHIPMENT       0x08
#define SUBCMD_SPI_READ           0x10
#define SUBCMD_SPI_WRITE          0x11
#define SUBCMD_SPI_SECTOR_ERASE   0x12
#define SUBCMD_RESET_NFC_IR_MCU   0x20
#define SUBCMD_SET_NFC_IR_CONFIG  0x21
#define SUBCMD_SET_NFC_IR_STATE   0x22
#define SUBCMD_SET_UNK_DATA       0x24
#define SUBCMD_RESET_UNK_DATA     0x25
#define SUBCMD_SET_UNK_NFCIR_DATA 0x28
#define SUBCMD_GET_NFCIR_MCU_DATA 0x29
#define SUBCMD_SET_GPIO_OUT_P2    0x2A
#define SUBCMD_GET_NFCIR_MCU_DATA_x29 0x2B

#define SUBCMD_SET_PLAYER_LIGHTS  0x30
#define SUBCMD_GET_PLAYER_LIGHTS  0x31
#define SUBCMD_SET_HOME_LIGHT     0x38
#define SUBCMD_TOGGLE_IMU         0x40
#define SUBCMD_IMU_SENSITIVITY    0x41
#define SUBCMD_IMU_REG_WRITE      0x42
#define SUBCMD_ENABLE_VIBRATION   0x48
#define SUBCMD_GET_REGULATED_VOLT 0x50
#define SUBCMD_SET_GPIO_OUT_P1    0x51
#define SUBCMD_GET_GPIO_IN_OUT    0x52


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

// 14  15  16  17  18  19  // 0
// 20  21  22  23  24  25  // 6, 12

// 26  27  28  29  30  31  // 12
// 32  33  34  35  36  37  // 18, 24

// 38  39  40  41  42  43  // 24
// 44  45  46  47  48  49  // 28, 36

static inline void switch_set_accelerometer(uint8_t *imu, size_t sample, int16_t x, int16_t y, int16_t z)
{
    if(sample > 2)
        return;

    analogue_int16_to_le(x, imu + (sample * 12));
    analogue_int16_to_le(y, imu + (sample * 12) + 2);
    analogue_int16_to_le(z, imu + (sample * 12) + 4);
}

static inline void switch_set_gyroscope(uint8_t *imu, size_t sample, int16_t x, int16_t y, int16_t z)
{
    if(sample > 2)
        return;

    analogue_int16_to_le(x, imu + (sample * 12) + 6);
    analogue_int16_to_le(y, imu + (sample * 12) + 6 + 2);
    analogue_int16_to_le(z, imu + (sample * 12) + 6 + 4);
}

enum NxBtButtons
{
    BUTTON_Y               = 0x00010000,
    BUTTON_X               = 0x00020000,
    BUTTON_B               = 0x00040000,
    BUTTON_A               = 0x00080000,
    BUTTON_JCR_SR          = 0x00100000,
    BUTTON_JCR_SL          = 0x00200000,
    BUTTON_R               = 0x00400000,
    BUTTON_ZR              = 0x00800000,

    BUTTON_PLUS            = 0x00000100,
    BUTTON_MINUS           = 0x00000200,
    BUTTON_R_STICK_PRESS   = 0x00000400,
    BUTTON_L_STICK_PRESS   = 0x00000800,
    BUTTON_HOME            = 0x00001000,
    BUTTON_CAPTURE         = 0x00002000,
    BUTTON_PADDING         = 0x00004000,
    BUTTON_CHARGING        = 0x00008000,

    BUTTON_DPAD_DOWN       = 0x00000001,
    BUTTON_DPAD_UP         = 0x00000002,
    BUTTON_DPAD_RIGHT      = 0x00000004,
    BUTTON_DPAD_LEFT       = 0x00000008,
    BUTTON_JCL_SR          = 0x00000010,
    BUTTON_JCL_SL          = 0x00000020,
    BUTTON_L               = 0x00000040,
    BUTTON_ZL              = 0x00000080,

    BUTTON_BEGIN = BUTTON_DPAD_DOWN,
    BUTTON_END = (BUTTON_ZR << 1)
};

enum NxReportMode
{
    // Used with command `x11`. Active polling for NFC/IR camera data. 0x31 data format must be set first.
    REPORT_MODE_POLLING_NFC_IR_CAM_DATA = 0x00,
    // Same as `00`. Active polling mode for NFC/IR MCU configuration data.
    REPORT_MODE_POLLING_NFC_IR_CAM_MCU_CFG = 0x01,
    // Same as `00`. Active polling mode for NFC/IR data and configuration. For specific NFC/IR modes
    REPORT_MODE_POLLING_NFC_IR_DATA_N_CFG = 0x02,
    // Same as `00`. Active polling mode for IR camera data. For specific IR modes
    REPORT_MODE_POLLING_IR_CAM_DATA = 0x03,
    // MCU update state report?
    REPORT_MODE_MCU_STATE = 0x23,
    // Standard full mode. Pushes current state @60Hz
    REPORT_MODE_STANDARD_FULL = 0x30,
    // NFC/IR mode. Pushes large packets @60Hz
    REPORT_NFC_IR_MODE = 0x31,
    // UNKNOWN 0x33
    REPORT_UNK_x33  = 0x33,
    // UNKNOWN 0x35
    REPORT_UNK_x35  = 0x35,
    // Simple HID mode. Pushes updates with every button press
    REPORT_SIMPLE_HID  = 0x3F,
};

enum NcReportHCIState
{
    HCI_STATE_DISCONNECT = 0x00,
    HCI_STATE_REBOOT_AND_RECONNECT = 0x01,
    HCI_STATE_REBOOT_TO_PAIRING = 0x02,
    HCI_STATE_REBOOT_AND_RECONNECT_HOME = 0x04
};

typedef struct
{
    uint32_t buttons;
    uint16_t stick_l[2];
    uint16_t stick_r[2];

    // Accelerometer
    int16_t  accel[3];
    uint8_t  accel_count;

    // Gyroscope
    int16_t  gyro[3];
    uint8_t  gyro_count;
} switch_pro_input_state_t;


typedef struct __attribute__((packed))
{
    uint8_t connection_info : 4;
    uint8_t battery_level   : 4;

    uint8_t m_button_status[3];

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
    bool     reports_suspended;
    bool     imu_enabled;           // Enable sending of the IMU data
    bool     vibration_enabled;     // Enable receiving vibration
    uint8_t  input_mode;            // Input report mode
    uint16_t button_elapsed[7];
    uint8_t  hci_state_recv;
    uint8_t  hci_state;

    uint8_t  gpio_p3_2;
    uint8_t  gpio_p0_4;
    uint8_t  gpio_p1_7;
    uint8_t  gpio_p1_15;
    // Port 2
    uint8_t  gpio_p2_2;

    uint8_t  player_id;             // player LED state

    uint8_t  imu_data_input[36];    // Input IMU samples, gets copied into report when imu is enabled
    uint8_t  imu_sense[4];          // IMU sensitivity state

    uint8_t  home_light[25];        // Home light PWM setup
    uint8_t  home_light_len;

    uint8_t  report_counter;        // incrementing time stamp for 0x30
    uint8_t  report_buf[64];        // scratch buffer for responses
} switch_pro_state_t;

#endif /* SOURCES_SWITCH_SWITCH_DESC_H_ */
