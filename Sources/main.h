/*
 * main.h
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_MAIN_H_
#define SOURCES_MAIN_H_

#include "stm32f1xx_hal.h"

void Error_Handler(void);

//#define user_button_Pin GPIO_PIN_13
//#define user_button_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC


#endif /* SOURCES_MAIN_H_ */
