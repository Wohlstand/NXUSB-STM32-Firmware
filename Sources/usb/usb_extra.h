/*
 * usb_extra.h
 *
 *  Created on: 12 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_USB_EXTRA_H_
#define SOURCES_USB_EXTRA_H_

#include <stm32f1xx_hal_gpio.h>

#define USBPU_port  GPIOA
#define USBPU_pin   GPIO_PIN_13
#define USB_CONNECT_STATE     0

#define USBPU_ON()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET)
#define USBPU_OFF() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET)

extern void USB_setup();

#endif /* SOURCES_USB_EXTRA_H_ */
