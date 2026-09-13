/*
 * usb.c
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#include "usb.h"
#include "tusb_config.h"
#include "tusb.h"
#include "debug_uart.h"


/* USB init function */

void MX_USB_PCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_USB_CLK_ENABLE();
}


/**
  * @brief This function handles USB FS global interrupt.
  */
void USB_LP_CAN_RX0_IRQHandler(void)
{
    tud_int_handler(0);
}

void USB_HP_CAN_TX_IRQHandler(void)
{
    tud_int_handler(0);
}
