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

#include <string.h>

#include "tusb_config.h"
#include "tusb.h"
#include "switch_desc.h"
#include "switch.h"
#include "../debug_uart.h"


// ---- Pro Controller protocol state ----
switch_pro_state_t pro_state;

static switch_pro_input_state_t input_state;

// ---- SPI Flash Data (factory calibration / configuration) ----
// Based on GP2040-CE and SwitchDualShockAdapter — addresses the Switch reads

// 0x6000: Serial number area (16 bytes of 0xFF = no serial)
static const uint8_t spi_6000[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // +0x10: padding
    0xFF, 0xFF,
    // +0x12: device type (0x03 = Pro Controller)
    0x03,
    // +0x13: unknown (usually 0xA0)
    0xA0,
    // +0x14..0x1A: unknown
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // +0x1B: colour info (0x02 = has grip colours)
    0x02,
    // +0x1C..0x1F: unknown
    0xFF, 0xFF, 0xFF, 0xFF,
    // +0x20..0x37: left stick factory calibration configuration/parameters
    0xE3, 0xFF, 0x39, 0xFF, 0xED, 0x01, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x09, 0x00, 0xEA, 0xFF,
    0xA1, 0xFF, 0x3B, 0x34, 0x3B, 0x34, 0x3B, 0x34,
    // +0x38..0x3C
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // +0x3D..0x45: left stick user calibration (9 bytes)
    0xA4, 0x46, 0x6A, 0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A,
    // +0x46..0x4E: right stick user calibration (9 bytes)
    0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A, 0xA4, 0x46, 0x6A,
    // +0x4F
    0xFF,
    // +0x50..0x52: body colour (dark grey)
    0x1B, 0x1B, 0x1D,
    // +0x53..0x55: button colour (white)
    0xFF, 0xFF, 0xFF,
    // +0x56..0x58: left grip colour
    0xEC, 0x00, 0x8C,
    // +0x59..0x5B: right grip colour
    0xEC, 0x00, 0x8C,
    // +0x5C
    0x01
};

// 0x6080: Stick parameters (factory) — left stick
static const uint8_t spi_6080[] =
{
    0x50, 0xFD, 0x00, 0x00, 0xC6, 0x0F,
    0x0F, 0x30, 0x61, 0xAE, 0x90, 0xD9, 0xD4, 0x14,
    0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33,
    0x36, 0x63
};

// 0x6098: Stick parameters (factory) — right stick
static const uint8_t spi_6098[] =
{
    0x0F, 0x30, 0x61, 0xAE, 0x90, 0xD9, 0xD4, 0x14,
    0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33,
    0x36, 0x63
};

// 0x8010: User calibration area (all 0xFF = no user calibration)
static const uint8_t spi_8010[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xB2, 0xA1
};

// 0x8028: User stick calibration
static const uint8_t spi_8028[] =
{
    0xBE, 0xFF, 0x3E, 0x00, 0xF0, 0x01,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0xFE, 0xFF, 0xFE, 0xFF,
    0x08, 0x00, 0xE7, 0x3B, 0xE7, 0x3B, 0xE7, 0x3B
};

// ---- Device Info ----
static const uint8_t device_info[] =
{
    0x03, 0x48,  // FW version
    0x03,        // Controller type: Pro Controller
    0x02,        // Unknown
    0xC7, 0xA3, 0x22, 0x53, 0x23, 0x43,  // MAC address (reversed)
    0x03,        // Unknown
    0x01         // Use colours
};

// MAC address for identify command (normal order)
static const uint8_t mac_address[] = {0x43, 0x23, 0x53, 0x22, 0xA3, 0xC7};



// ---- Helper: read SPI flash data ----
static void read_spi_flash(uint8_t *dest, uint32_t address, uint8_t size)
{
    // Try to serve from our static data tables
    // We handle reads by base address range
    const uint8_t *src = NULL;
    uint32_t src_size = 0;
    uint32_t base = 0;

    if(address >= 0x6000 && address < 0x6000 + sizeof(spi_6000))
    {
        src = spi_6000;
        src_size = sizeof(spi_6000);
        base = 0x6000;
    }
    else if(address >= 0x6080 && address < 0x6080 + sizeof(spi_6080))
    {
        src = spi_6080;
        src_size = sizeof(spi_6080);
        base = 0x6080;
    }
    else if(address >= 0x6098 && address < 0x6098 + sizeof(spi_6098))
    {
        src = spi_6098;
        src_size = sizeof(spi_6098);
        base = 0x6098;
    }
    else if(address >= 0x8010 && address < 0x8010 + sizeof(spi_8010))
    {
        src = spi_8010;
        src_size = sizeof(spi_8010);
        base = 0x8010;
    }
    else if(address >= 0x8028 && address < 0x8028 + sizeof(spi_8028))
    {
        src = spi_8028;
        src_size = sizeof(spi_8028);
        base = 0x8028;
    }

    if(src != NULL)
    {
        uint32_t offset = address - base;
        uint8_t avail = (uint8_t)(src_size - offset);
        uint8_t copy_size = (size < avail) ? size : avail;
        memcpy(dest, src + offset, copy_size);
        if(copy_size < size)
        {
            memset(dest + copy_size, 0xFF, size - copy_size);
        }
    }
    else
    {
        // Unknown address — return 0xFF (no data)
        memset(dest, 0xFF, size);
    }
}

// ---- Helper: fill input sub-report into buffer (neutral position) ----
static void fill_input_subreport(uint8_t *buf)
{
    // 11 bytes of switch_input_report_t at neutral
    memset(buf, 0, 11);
    buf[0] = 0x08;  // battery level = 8 (good), connection = 0 (USB)

    // Sticks at centre: 0x7FF for both X and Y (12-bit)
    // Byte layout: [0]=batt/conn, [1..3]=buttons, [4..6]=left stick, [7..9]=right stick
    // 0x7FF packed: {0xFF, 0xF7, 0x7F} matching GP2040-CE
    buf[4] = 0xFF; buf[5] = 0xF7; buf[6] = 0x7F;
    buf[7] = 0xFF; buf[8] = 0xF7; buf[9] = 0x7F;

    // Vibration status
    if(pro_state.vibration_enabled)
        buf[10] = pro_state.vibration_status;
}

// ---- Queue a response report ----
static bool report_queued = false;
static uint8_t queued_report_id = 0;
static uint32_t queued_report_delayed = 0;
static const char* queued_report_debug_id = "UNK";
static uint32_t queued_report_last_delay = 0;

static void queue_response(uint8_t report_id, uint32_t delay, const char *debug_id)
{
    queued_report_id = report_id;
    queued_report_debug_id = debug_id;
    report_queued = true;
    queued_report_delayed = delay;
    queued_report_last_delay = 0;
}

// ---- Handle 0x80 config commands ----
static void handle_config_command(uint8_t subcmd)
{
    uint8_t *buf = pro_state.report_buf;
    memset(buf, 0, 64);

    // debug_print("[80] cmd="); debug_hex8(subcmd); debug_print("\r\n");

    switch (subcmd)
    {
    case SUBCMD_80_IDENTIFY:
        // Reply: 0x81 0x01 + controller type + MAC
        buf[0] = REPORT_ID_INPUT_81;
        buf[1] = SUBCMD_80_IDENTIFY;
        buf[2] = 0x00;
        buf[3] = 0x03;  // Pro Controller
        memcpy(&buf[4], mac_address, 6);
        queue_response(0, 0, "IDENT");
        break;

    case SUBCMD_80_HANDSHAKE:
        buf[0] = REPORT_ID_INPUT_81;
        buf[1] = SUBCMD_80_HANDSHAKE;
        queue_response(0, 0, "HSHAKE");
        break;

    case SUBCMD_80_BAUD_RATE:
        buf[0] = REPORT_ID_INPUT_81;
        buf[1] = SUBCMD_80_BAUD_RATE;
        queue_response(0, 0, "BAUD");
        break;

    case SUBCMD_80_DISABLE_USB_TIMEOUT:
        // This means "switch to full reporting mode"
        // debug_println("[80] READY - reports enabled");
        pro_state.handshake_done = true;
        pro_state.reports_enabled = true;
        buf[0] = REPORT_ID_INPUT_30;
        buf[1] = subcmd;
        queue_response(0, 0, "DISUSBTO");
        debug_println("[USB] Device is ready!");
        break;

    case SUBCMD_80_ENABLE_USB_TIMEOUT:
        buf[0] = REPORT_ID_INPUT_30;
        buf[1] = subcmd;
        queue_response(0, 0, "ENUSBTO");
        break;

    default:
        buf[0] = REPORT_ID_INPUT_30;
        buf[1] = subcmd;
        queue_response(0, 0, "Other");
        break;
    }
}

static const uint8_t c_rumbleNeutral[8] = {0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40};

static void vibration_cycle(void)
{
    // Update status at 0x08 and up to 0x0C
    if(pro_state.vibration_status == 0x08)
        pro_state.vibration_status = 0x09;
    else if(pro_state.vibration_status == 0x09)
        pro_state.vibration_status = 0x0A;
    else if(pro_state.vibration_status == 0x0A)
        pro_state.vibration_status = 0x0B;
    else if(pro_state.vibration_status == 0x0B)
        pro_state.vibration_status = 0x0C;
}

static void handle_rumble(const uint8_t *data, uint16_t len)
{
    if(memcmp(data + 2, c_rumbleNeutral, 8) == 0)
    {
        if(pro_state.vibration_last_neutral)
            return;

        pro_state.vibration_last_neutral = true;
    }
    else
        pro_state.vibration_last_neutral = false;

    debug_rumble(data + 2, 8);

    pro_state.vibration_status = 0x08;
}

// ---- Handle 0x01 UART subcommands ----
static void handle_subcommand(const uint8_t *data, uint16_t len)
{
    uint8_t *buf = pro_state.report_buf;
    memset(buf, 0, 64);

    // 0x21 reply frame:
    // [0] = 0x21 (report ID)
    // [1] = timestamp
    // [2..12] = input sub-report (11 bytes)
    // [13] = ACK byte (0x80 | subcmd if data follows, 0x80 if ACK only)
    // [14] = subcmd echo
    // [15+] = reply data

    // If rumble data is not zeroed and vibration IS enabled
    if(pro_state.vibration_enabled)
        handle_rumble(data , 10);

    buf[0] = REPORT_ID_INPUT_21;
    buf[1] = pro_state.report_counter++;
    fill_input_subreport(&buf[2]);

    uint8_t subcmd = data[10]; // subcommand ID is at offset 10
    uint8_t sublen = len > 11 ? len - 11 : 0;

    debug_print_begin();
    debug_insert("[01] sub="); debug_hex8(subcmd);
    debug_print_end();

    switch(subcmd)
    {
    case SUBCMD_GET_STATE:
        buf[13] = 0x80 | subcmd;
        buf[14] = subcmd;
        buf[15] = 0x03;
        break;

    case SUBCMD_BT_PAIR:
        buf[13] = 0x80 | subcmd;
        buf[14] = subcmd;
        buf[15] = 0x03;
        buf[16] = 0x01;
        break;

    case SUBCMD_DEVICE_INFO:
        buf[13] = 0x82;
        buf[14] = SUBCMD_DEVICE_INFO;
        memcpy(&buf[15], device_info, sizeof(device_info));
        break;

    case SUBCMD_SET_MODE:
        if(sublen > 0)
            pro_state.input_mode = data[11];
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_TRIGGER_BUTTONS:
        if(sublen >= 2)
        {
            size_t i = 0;
            for(i = 0; i < 7; ++i)
                pro_state.button_elapsed[i] = ((data[12u + (i * 2)] << 8) | data[11u + (i * 2)]) * 10u;
        }
        buf[13] = 0x83;
        buf[14] = SUBCMD_TRIGGER_BUTTONS;
        break;

    case SUBCMD_GET_PAGELIST_STATE:
        // Replies a uint8 with a value of `x01` if there's a Host list with BD addresses/link keys in memory.
        buf[13] = 0x80;
        buf[14] = subcmd;
        buf[15] = 0x00;
        break;

    case SUBCMD_SET_HCI_STATE:
        if(sublen > 0)
        {
            pro_state.hci_state_recv = 1;
            pro_state.hci_state = data[11];
        }
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_RESET_PAIR_INFO:
        // Initialises the 0x2000 SPI section.
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_SET_SHIPMENT:
        // Subcommand 0x08: Set shipment low power state
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_SPI_READ:
    {
        // data[11..14] = address (little-endian 32-bit)
        // data[15] = size
        uint32_t addr = (uint32_t)data[11] | ((uint32_t)data[12] << 8) |
                        ((uint32_t)data[13] << 16) | ((uint32_t)data[14] << 24);
        uint8_t size = data[15];
        // debug_print_begin();
        // debug_print("[SPI] addr="); debug_hex32(addr); debug_print(" sz="); debug_hex8(size); debug_print("\r\n");
        // debug_print_end();

        buf[13] = 0x90;
        buf[14] = subcmd;
        // Echo back the address and size
        buf[15] = data[11];
        buf[16] = data[12];
        buf[17] = data[13];
        buf[18] = data[14];
        buf[19] = size;

        // Read SPI flash data into buf[20..]
        if(size > 44)
            size = 44;  // clamp to prevent overflow

        read_spi_flash(&buf[20], addr, size);
        break;
    }

    case SUBCMD_SPI_WRITE:
        // Little-endian int32 address, int8 size. Max size `x1D` data to write.
        // Replies with `x8011` ACK and a uint8 status. `x00` = success, `x01` = write protected.
        buf[13] = 0x80;
        buf[14] = subcmd;
        buf[15] = 0x01; // Always write protected!, 0x00 is success
        break;

    case SUBCMD_SPI_SECTOR_ERASE:
        // Takes a Little-endian uint32. Erases the whole 4KB in the specified address to 0xFF.
        // Replies with `x8012` ACK and a uint8 status. `x00` = success, `x01` = write protected.
        buf[13] = 0x80;
        buf[14] = subcmd;
        buf[15] = 0x01; // Always write protected!, 0x00 is success
        break;

    case SUBCMD_RESET_NFC_IR_MCU:
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_SET_NFC_IR_CONFIG:
        // Write configuration data to MCU. This data can be IR configuration, NFC configuration or data for the 512KB MCU firmware update.
        // Takes 38 or 37 bytes long argument data.
        // Replies with ACK `xA0` `x20` and 34 bytes of data.
        buf[13] = 0x80; //0xA0;
        buf[14] = subcmd; // 0x20;
        memset(buf + 15, 0, 34);
        break;

    case SUBCMD_SET_NFC_IR_STATE:
        // Takes one argument:
        //
        // | Argument # | Remarks           |
        // |:----------:| ----------------- |
        // |   `00`     | Suspend           |
        // |   `01`     | Resume            |
        // |   `02`     | Resume for update |
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_SET_UNK_DATA:
        // Takes a 38 byte long argument.
        // Sets a byte to `x01` (enable something?) and sets also an unknown data
        // (configuration? for NFC/IR MCU?) to the BT device structure that copies it from given argument.
        // Replies with `x80 24 00` always.
        buf[13] = 0x80;
        buf[14] = subcmd;
        buf[15] = 0x00;
        break;

    case SUBCMD_RESET_UNK_DATA:
        // Sets the above byte to `x00` (disable something?) and resets the previous 38 byte data to all zeroes.
        // Replies with `x80 25 00` always.
        buf[13] = 0x80;
        buf[14] = subcmd;
        buf[15] = 0x00;
        break;

    case SUBCMD_SET_UNK_NFCIR_DATA:
        // Takes a 38 byte long argument and copies it to unknown array_222640[96] at &array_222640[3].
        // Does the same job with OUTPUT report 0x12.
        // Replies with ACK `x80` `x28`.
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_GET_NFCIR_MCU_DATA:
        // Replies with ACK `xA8` `x29` and 34 bytes data, from a different buffer than the one the x28 writes.
        buf[13] = 0xA8;
        buf[14] = 0x29;
        break;

    case SUBCMD_SET_GPIO_OUT_P2:
        // Takes a uint8_t and sets unknown GPIO Pin 2 at Port 2 to `0` = GPIO_PIN_OUTPUT_LOW` or `1` = GPIO_PIN_OUTPUT_HIGH`.
        // This normally enables a function. For example, sub-cmd `x48` sets GPIO Pin 7 @Port 2 output value, which disables or enables IMU.
        // Replies always with ACK `x00` `x2A`.
        // `x00` as an ACK here is a bug. Developers forgot to add an ACK reply.
        if(sublen > 0)
            pro_state.gpio_p2_2 = data[11];
        buf[13] = 0x00;
        buf[14] = subcmd;

    case SUBCMD_GET_NFCIR_MCU_DATA_x29:
        // Replies with ACK `xA9` `x2B` and 20 bytes long data (which has also a part from x24 sub-cmd).
        buf[13] = 0xA9;
        buf[14] = 0x29;
        break;

    case SUBCMD_SET_PLAYER_LIGHTS:
        // First argument byte is a bit-field:
        // ```
        // aaaa bbbb
        //      3210 - keep player light on
        // 3210 - flash player light
        // ```
        pro_state.player_id = data[11];
        buf[13] = 0x80;
        buf[14] = subcmd;
        debug_palyer(pro_state.player_id);
        break;

    case SUBCMD_GET_PLAYER_LIGHTS:
        buf[13] = 0xB1;
        buf[14] = subcmd;
        buf[15] = pro_state.player_id;
        break;

    case SUBCMD_TOGGLE_IMU:
        pro_state.imu_enabled = data[11] == 0x01 ? true : false;
        buf[13] = 0x80;
        buf[14] = subcmd;
        debug_imu_state((uint8_t)pro_state.imu_enabled);
        break;

    case SUBCMD_IMU_SENSITIVITY:
        if(sublen >= 4)
        {
            memcpy(pro_state.imu_sense, data + 11, sublen >= 4 ? 4 : sublen);
            debug_imu_sens(pro_state.imu_sense, 4);
        }
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_IMU_REG_WRITE:
        if(sublen >= 3)
        {
            // Send raw registers commands to the LSM6DS3 chip which implements gyroscope and accelerometer
            debug_imu_reg_write(data + 11, sublen >= 3 ? 3 : sublen);
        }
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_ENABLE_VIBRATION:
        pro_state.vibration_enabled = data[11] == 0x01 ? true : false;
        buf[13] = 0x80;
        buf[14] = subcmd;
        debug_vibro((uint8_t)pro_state.vibration_enabled);
        break;

    case SUBCMD_SET_HOME_LIGHT:
        if(sublen > 0)
        {
            pro_state.home_light_len = sublen >= 25 ? 25 : sublen;
            memcpy(pro_state.home_light, data + 11, pro_state.home_light_len);
            debug_home_light(pro_state.home_light, pro_state.home_light_len);
        }
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_GET_REGULATED_VOLT:
        buf[13] = 0xD0;
        buf[14] = subcmd;
        buf[15] = 0x90; // 0x0690
        buf[16] = 0x06;
        break;

    case SUBCMD_SET_GPIO_OUT_P1:
        if(sublen > 0)
        {
            pro_state.gpio_p1_7 = (data[11] & 0x04) == 0;
            pro_state.gpio_p1_15 = (data[11] & 0x10) != 0;
        }
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;

    case SUBCMD_GET_GPIO_IN_OUT:
        buf[13] = 0xD1;
        buf[14] = subcmd;
        buf[15] = (pro_state.gpio_p0_4 ? 0 : 0x01) |
                  (pro_state.gpio_p3_2 ? 0 : 0x02) |
                  (pro_state.gpio_p1_7 ? 0 : 0x04) |
                  (pro_state.gpio_p1_15 ? 0x08 : 0);
        break;

    default:
        // Unknown sub-commands — plain ACK
        buf[13] = 0x80;
        buf[14] = subcmd;
        break;
    }

    //debug_print("  -> ACK="); debug_hex8(buf[13]);
    // debug_print(" sub="); debug_hex8(buf[14]); debug_print("\r\n");
    queue_response(0, 0, "SubCMD");
}


// ---- Public: process incoming output report from Switch ----
void switch_pro_handle_output(const uint8_t *data, uint16_t len)
{
    if(len < 2)
        return;

    // debug_dump("<< IN", data, (len > 20) ? 20 : len);

    uint8_t report_id = data[0];
    uint8_t sub_id    = data[1];

    switch (report_id)
    {
    case REPORT_ID_OUTPUT_80:
        handle_config_command(sub_id);
        break;

    case REPORT_ID_OUTPUT_01:
        if(len >= 11)
            handle_subcommand(data, len);
        break;

    case REPORT_ID_OUTPUT_10:
        if(len >= 10)
            handle_rumble(data, len);
        break;
    default:
        break;
    }
}

void switch_input_state_reset(void)
{
    memset(&input_state, 0, sizeof(input_state));

    // Centred sticks
    input_state.stick_l[0] = 0x7FF;
    input_state.stick_l[1] = 0x7FF;
    input_state.stick_r[0] = 0x7FF;
    input_state.stick_r[1] = 0x7FF;
}

void switch_input_set_defaults(void)
{
    pro_state.player_id = 0;
    pro_state.imu_enabled = false;
    pro_state.vibration_enabled = false;

    pro_state.vibration_status = 0x00;
    pro_state.vibration_last_neutral = false;

    pro_state.imu_sense[0] = 0x03;
    pro_state.imu_sense[1] = 0x00;
    pro_state.imu_sense[2] = 0x00;
    pro_state.imu_sense[3] = 0x01;

    switch_query_config();

    pro_state.input_mode = REPORT_MODE_STANDARD_FULL;
}

void switch_init_input_state(void)
{
    memset(&pro_state, 0, sizeof(input_state));
    switch_input_state_reset();
}

// ---- TinyUSB callbacks ----
void tud_mount_cb(void)
{
//    debug_println("[USB] MOUNTED");
    pro_state.handshake_done = false;
    pro_state.reports_enabled = false;
    pro_state.reports_suspended = false;
    pro_state.report_counter = 0;
    report_queued = false;
    switch_input_state_reset();
    switch_input_set_defaults();
}

void switch_query_player(void)
{
    debug_palyer(pro_state.player_id);
}

void switch_query_imu(void)
{
    debug_imu_state(pro_state.imu_enabled);
}

void switch_query_vibro(void)
{
    debug_vibro(pro_state.vibration_enabled);
}

void switch_query_config(void)
{
    uint8_t msg[3];

    msg[0] = pro_state.player_id;
    msg[1] = pro_state.imu_enabled;
    msg[2] = pro_state.vibration_enabled;

    debug_config(msg, sizeof(msg));
}

void tud_umount_cb(void)
{
//    debug_println("[USB] UNMOUNTED");
    pro_state.handshake_done = false;
    pro_state.reports_enabled = false;
    pro_state.reports_suspended = false;
    switch_input_set_defaults();
    switch_input_state_reset();
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    debug_println("[USB] Suspend");
    pro_state.reports_suspended = true;
    switch_input_set_defaults();
}

void tud_resume_cb(void)
{
    debug_println("[USB] Resume");
    pro_state.reports_suspended = false;
}

void dcd_disconnect(uint8_t rhport)
{
    (void)rhport;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
    debug_println("[USB] Soft-Disconnect");
}

void dcd_connect(uint8_t rhport)
{
    (void)rhport;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
    debug_println("[USB] Soft-Connect");
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)report;
    (void)len;
//    debug_println("[USB] Complete Report Request");
}

// GET_REPORT — return empty/neutral
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen)
{
    (void)itf; (void)report_id; (void)report_type; (void)reqlen;

    debug_println("[USB] tud_hid_get_report_cb");

    return 0;
}

// SET_REPORT — Switch sends commands via the OUT endpoint
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const* buffer, uint16_t bufsize)
{
    (void)itf; (void)report_id; (void)report_type;

//    debug_println("[USB] tud_hid_set_report_cb");

    switch_pro_handle_output(buffer, bufsize);
}

// ---- Called from main loop to send queued responses ----
bool switch_pro_send_queued(void)
{
    if(!report_queued)
        return false;

    if(queued_report_delayed > 0)
    {
        int32_t now = HAL_GetTick();

        if(queued_report_last_delay == 0)
            queued_report_last_delay = now;

        if(now - queued_report_last_delay < queued_report_delayed)
            return false;

        queued_report_last_delay = 0;
        queued_report_delayed = 0;
    }

    if(!tud_hid_ready())
        return false;

    debug_print_begin();
    debug_insert("[USB] Queued ReportID=");
    debug_hex8(queued_report_id);
    debug_insert(" type ");
    debug_insert(queued_report_debug_id);
    debug_print_end();
    debug_dump(">> OUT", pro_state.report_buf, 20);

    tud_hid_report(queued_report_id, pro_state.report_buf, 64);

    report_queued = false;
    return true;
}

bool switch_pro_active(void)
{
    return pro_state.handshake_done && !pro_state.reports_suspended;
}

void switch_receive(const struct NXSendCmd *in)
{
    switch(in->cmd)
    {
    case CMD_ButtonsUpdate:
        input_state.buttons = in->data.state.buttons;
        memcpy(input_state.stick_l, in->data.state.stick_l, sizeof(uint16_t) * 2);
        memcpy(input_state.stick_r, in->data.state.stick_r, sizeof(uint16_t) * 2);
        break;

    case CMD_Tilt:
        memcpy(input_state.accel, in->data.tilt.accel, sizeof(int16_t) * 3);
        memcpy(input_state.gyro, in->data.tilt.gyro, sizeof(int16_t) * 3);

        switch_set_accelerometer(pro_state.imu_data_input, 0, input_state.accel[0], input_state.accel[1], input_state.accel[2]);
        switch_set_accelerometer(pro_state.imu_data_input, 1, input_state.accel[0], input_state.accel[1], input_state.accel[2]);
        switch_set_accelerometer(pro_state.imu_data_input, 2, input_state.accel[0], input_state.accel[1], input_state.accel[2]);

        switch_set_gyroscope(pro_state.imu_data_input, 0, input_state.gyro[0], input_state.gyro[1], input_state.gyro[2]);
        switch_set_gyroscope(pro_state.imu_data_input, 1, input_state.gyro[0], input_state.gyro[1], input_state.gyro[2]);
        switch_set_gyroscope(pro_state.imu_data_input, 2, input_state.gyro[0], input_state.gyro[1], input_state.gyro[2]);

        break;

    default:
        break;
    }
}


void switch_update_state(void)
{
    if(!pro_state.reports_enabled || pro_state.reports_suspended)
        return;

    static uint32_t last_report_ms = 0;
    uint32_t now = HAL_GetTick();

    if(pro_state.hci_state_recv)
    {
        pro_state.hci_state_recv = 0;

        switch(pro_state.hci_state)
        {
        case HCI_STATE_DISCONNECT:
            debug_println("HCI Requested Disconnect");
            break;

        case HCI_STATE_REBOOT_AND_RECONNECT:
            debug_println("HCI Requested Reboot and Reconnect");
            break;

        case HCI_STATE_REBOOT_TO_PAIRING:
            debug_println("HCI Requested Reboot and Pairing");
            break;

        case HCI_STATE_REBOOT_AND_RECONNECT_HOME:
            debug_println("HCI Requested Reboot and Reconnect (HOME)");
            break;
        }
    }

    // Send input reports every 8ms (~125 Hz)
    if(tud_hid_ready() && (now - last_report_ms >= 8))
    {
        last_report_ms = now;

        // Initialise the vibration engine
        if(!pro_state.vibration_last_neutral && pro_state.vibration_status != 0x00)
            vibration_cycle();

        switch_pro_report_t report;
        memset(&report, 0, sizeof(report));
        report.report_id = REPORT_ID_INPUT_30;
        report.timestamp = pro_state.report_counter++;

        // Battery good, USB connection
        report.input.battery_level = 0x08;
        report.input.connection_info = 0x00;
//        report.input.charging = 1;

        switch_analog_set_xy(&report.input.left_stick,  input_state.stick_l[0], input_state.stick_l[1]);
        switch_analog_set_xy(&report.input.right_stick, input_state.stick_r[0], input_state.stick_r[1]);

        if(pro_state.imu_enabled)
            memcpy(report.imu_data, pro_state.imu_data_input, sizeof(report.imu_data));

        // Vibro-motor report
        report.rumble_report = pro_state.vibration_status;

        report.input.m_button_status[0] = (input_state.buttons >> 16) & 0xFF;
        report.input.m_button_status[1] = ((input_state.buttons >> 8) & 0xFF) | ((BUTTON_CHARGING >> 8) & 0xFF);
        report.input.m_button_status[2] = (input_state.buttons) & 0xFF;

        tud_hid_report(0, &report, sizeof(report));
    }
}
