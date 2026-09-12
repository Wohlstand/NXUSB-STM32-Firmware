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


PCD_HandleTypeDef hpcd_USB_DRD_FS;

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

//    USB->CNTR   = 0;
//    USB->BTABLE = 0;
//    USB->DADDR  = 0;
//    USB->ISTR   = 0;
//    USB->CNTR   = USB_CNTR_RESETM | USB_CNTR_WKUPM;

//    hpcd_USB_DRD_FS.Instance = USB;
//    hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
//    hpcd_USB_DRD_FS.Init.dma_enable = 0;
//    hpcd_USB_DRD_FS.Init.speed = PCD_SPEED_FULL;
//    hpcd_USB_DRD_FS.Init.ep0_mps = 64;
//    hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
//    hpcd_USB_DRD_FS.Init.Sof_enable = ENABLE;
//    hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
//    hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
//    hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
//
//    if (HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK)
//    {
//        Error_Handler();
//    }
//    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
//    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
//    debug_println("USB setup");
//    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
//    USB->CNTR   = USB_CNTR_FRES; // Force USB Reset
//    for(uint32_t ctr = 0; ctr < 72000; ++ctr)
//        __NOP(); // wait >1ms
//    USB->CNTR   = 0;
//    USB->BTABLE = 0;
//    USB->DADDR  = 0;
//    USB->ISTR   = 0;
//    USB->CNTR   = USB_CNTR_RESETM | USB_CNTR_WKUPM | USB_CNTR_CTRM | USB_CNTR_ERRM | USB_CNTR_PDWN;
//    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
//    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
}


void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

//    if(pcdHandle->Instance == USB)
//    {
//        __HAL_RCC_GPIOA_CLK_ENABLE();
//        GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Set USB Interrupt priority */
//        NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 15);
//        NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 15);
//        NVIC_SetPriority(USBWakeUp_IRQn, 15);

        /* Enable USB Interrupt */
//        HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
//        HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
//        HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);

//        __HAL_RCC_USB_CLK_ENABLE();
//    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
//    if(pcdHandle->Instance == USB)
//    {
//        /* Peripheral clock disable */
//        __HAL_RCC_USB_CLK_DISABLE();
////        /**USB GPIO Configuration
////        PA8     ------> USB_SOF
////        PA11     ------> USB_DM
////        PA12     ------> USB_DP
////        */
////        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12);
//
//        /* USB_DRD_FS interrupt Deinit */
//        HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
//        HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
//    }
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

//void USBWakeUp_IRQHandler(void)
//{
//    tud_int_handler(BOARD_TUD_RHPORT);
//}

