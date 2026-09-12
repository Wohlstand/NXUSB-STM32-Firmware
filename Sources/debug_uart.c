#include "debug_uart.h"
#include <string.h>
//#include <stdbool.h>

static UART_HandleTypeDef *debug_huart = NULL;
static const char msg_debug = 'd';
static const char msg_player = 'p';
static const char msg_imu = 'i';
static const char msg_rumble = 'r';
static const char msg_tail = 0xFF;
//static const char msg_tail = '\n';

void debug_init(UART_HandleTypeDef *huart)
{
    debug_huart = huart;
    debug_println("\r\n--- Switch Pro Controller Debug ---");
}

//volatile bool s_print_busy = false;

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
//{
//    s_print_busy = false;
//}

extern void Error_Handler(void);

void debug_print_begin(void)
{
    if(!debug_huart)
        return;

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)&msg_debug, 1, 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_print_end(void)
{
    if(!debug_huart)
        return;

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)&msg_tail, 1, 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_print(const char *str)
{
    if (!debug_huart)
        return;

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)str, (uint16_t)strlen(str), 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_imu_state(uint8_t enabled)
{
    uint8_t msg[3];

    if(!debug_huart)
        return;

    msg[0] = msg_imu;
    msg[1] = enabled;
    msg[2] = msg_tail;

    if(HAL_UART_Transmit_IT(debug_huart, (const uint8_t *)msg, 3) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_palyer(uint8_t player)
{
    uint8_t msg[3];

    if(!debug_huart)
        return;

    msg[0] = msg_player;

    switch(player)
    {
    case 0x01:
    case 0x10:
        msg[1] = 1;
        break;

    case 0x03:
    case 0x30:
        msg[1] = 2;
        break;

    case 0x07:
    case 0x70:
        msg[1] = 3;
        break;

    case 0x0F:
    case 0xF0:
        msg[1] = 4;
        break;
    }

    msg[2] = msg_tail;

    if(HAL_UART_Transmit_IT(debug_huart, (const uint8_t *)msg, 3) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_rumble(uint8_t *samples, uint8_t size)
{
    if(!debug_huart)
        return;

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)&msg_rumble, 1, 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)samples, (uint16_t)size, 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }

    if(HAL_UART_Transmit(debug_huart, (const uint8_t *)&msg_tail, 1, 10) != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

static const char hex_chars[] = "0123456789ABCDEF";

void debug_hex8(uint8_t val)
{
    if (!debug_huart) return;
    char buf[3];
    buf[0] = hex_chars[(val >> 4) & 0x0F];
    buf[1] = hex_chars[val & 0x0F];
    buf[2] = '\0';
    debug_print(buf);
}

void debug_hex16(uint16_t val)
{
    debug_hex8((uint8_t)(val >> 8));
    debug_hex8((uint8_t)(val & 0xFF));
}

void debug_hex32(uint32_t val)
{
    debug_hex16((uint16_t)(val >> 16));
    debug_hex16((uint16_t)(val & 0xFFFF));
}

void debug_println(const char *str)
{
    debug_print_begin();
    debug_print(str);
    debug_print_end();
}

void debug_dump(const char *label, const uint8_t *data, uint8_t len)
{
    if (!debug_huart)
        return;

    debug_print_begin();
    debug_print(label);
    debug_print(": ");
    uint8_t n = (len > 64) ? 64 : len;

    for (uint8_t i = 0; i < n; i++)
    {
        debug_hex8(data[i]);
        debug_print(" ");
    }

    debug_print_end();
}
