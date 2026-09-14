#include "debug_uart.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
//#include <stdbool.h>

static UART_HandleTypeDef *debug_huart = NULL;
static const char msg_debug     = 'd';
static const char msg_player    = 'p';
static const char msg_imu       = 'i';
static const char msg_imusens   = 's';
static const char msg_imureg    = 'e';
static const char msg_config    = 'c';
static const char msg_rumble    = 'r';
static const char msg_home      = 'h';
static const char msg_vibro     = 'v';
static const char msg_tail      = 0x7F;
//static const char msg_tail = '\n';

static const size_t     log_buffer_max = 1024;
static uint16_t         log_buffer_length = 0;
static uint8_t          log_buffer[1024 + 1] = {0};


void mine_debug_print(const char *format, ...)
{
    char sub_buffer[1022];
    va_list list;

    va_start(list, format);
    vsnprintf(sub_buffer, 1022, format, list);
    va_end(list);

    debug_print_begin();
    debug_insert(sub_buffer);
    debug_print_end();
}


static void log_insert(const uint8_t *buff, uint16_t len)
{
    if(len + log_buffer_length >= log_buffer_max - 1)
        len = (log_buffer_max - 1) - log_buffer_length;

    if(len == 0)
        return; // Nothing to add

    memcpy(log_buffer + log_buffer_length, buff, len);
    log_buffer_length += len;
}

void debug_init(UART_HandleTypeDef *huart)
{
    debug_huart = huart;
    debug_println("--- Switch Pro Controller Debug ---");
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

    while(debug_huart->gState != HAL_UART_STATE_READY);

    log_buffer[0] = 0;
    log_buffer_length = 0;

    log_insert((const uint8_t*)&msg_debug, 1);
}

void debug_print_end(void)
{
    HAL_StatusTypeDef ret;

    if(!debug_huart)
        return;

    log_insert((const uint8_t*)&msg_tail, 1);

    while(debug_huart->gState != HAL_UART_STATE_READY);

    ret = HAL_UART_Transmit_IT(debug_huart, (const uint8_t *)&log_buffer, log_buffer_length);

    if(ret != HAL_OK)
    {
        log_buffer[0] = ret;
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_insert(const char *str)
{
    if (!debug_huart)
        return;

    log_insert((const uint8_t*)str, strlen(str));
}

static void debug_write_chunk(const char *head, const uint8_t *samples, uint8_t size)
{
    if(!debug_huart)
        return;

    while(debug_huart->gState != HAL_UART_STATE_READY);

    log_buffer[0] = 0;
    log_buffer_length = 0;

    log_insert((const uint8_t*)head, 1);
    log_insert(samples, (uint16_t)size);
    debug_print_end();
}

static void debug_write_byte(const char *head, uint8_t byte)
{
    HAL_StatusTypeDef ret;
    static uint8_t msg[3];

    if(!debug_huart)
        return;

    msg[0] = (uint8_t)*head;
    msg[1] = byte;
    msg[2] = msg_tail;

    while(debug_huart->gState != HAL_UART_STATE_READY);

    ret = HAL_UART_Transmit_IT(debug_huart, msg, sizeof(msg));

    if(ret != HAL_OK)
    {
        /* Transfer error in reception process */
        Error_Handler();
    }
}

void debug_imu_state(uint8_t enabled)
{
    debug_write_byte(&msg_imu, enabled);
}

void debug_imu_sens(const uint8_t *samples, uint8_t size)
{
    debug_write_chunk(&msg_imusens, samples, size);
}

void debug_imu_reg_write(const uint8_t *samples, uint8_t size)
{
    debug_write_chunk(&msg_imureg, samples, size);
}

void debug_vibro(uint8_t enabled)
{
    debug_write_byte(&msg_vibro, enabled);
}

void debug_palyer(uint8_t player)
{
    debug_write_byte(&msg_player, player);
}

void debug_config(const uint8_t *samples, uint8_t size)
{
    debug_write_chunk(&msg_config, samples, size);
}



void debug_rumble(const uint8_t *samples, uint8_t size)
{
    debug_write_chunk(&msg_rumble, samples, size);
}

void debug_home_light(const uint8_t *samples, uint8_t size)
{
    debug_write_chunk(&msg_home, samples, size);
}

static const char hex_chars[] = "0123456789ABCDEF";

void debug_hex8(uint8_t val)
{
    if (!debug_huart) return;
    char buf[3];
    buf[0] = hex_chars[(val >> 4) & 0x0F];
    buf[1] = hex_chars[val & 0x0F];
    buf[2] = '\0';
    debug_insert(buf);
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
    debug_insert(str);
    debug_print_end();
}

void debug_printf(const char *format, ...)
{
    char sub_buffer[1022];
    va_list list;

    va_start(list, format);
    vsnprintf(sub_buffer, 1022, format, list);
    va_end(list);

    debug_print_begin();
    debug_insert(sub_buffer);
    debug_print_end();
}

void debug_dump(const char *label, const uint8_t *data, uint8_t len)
{
    if (!debug_huart)
        return;

    debug_print_begin();

    debug_insert(label);
    debug_insert(": ");
    uint8_t n = (len > 64) ? 64 : len;

    for (uint8_t i = 0; i < n; i++)
    {
        debug_hex8(data[i]);
        debug_insert(" ");
    }

    debug_print_end();
}
