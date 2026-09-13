/*
 * usb.h
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_USB_USB_H_
#define SOURCES_USB_USB_H_

#include "../main.h"

extern PCD_HandleTypeDef hpcd_USB_DRD_FS;

extern void MX_USB_PCD_Init(void);

extern void usb_SetInitMode(uint8_t init);

#endif /* SOURCES_USB_USB_H_ */
