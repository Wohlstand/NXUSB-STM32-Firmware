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

static uint8_t init_mode = 0;

//// USB status & its address
//typedef struct
//{
//    uint8_t  USB_Status;
//    uint16_t USB_Addr;
//} usb_dev_t;
//
//ep_t endpoints[];
//usb_dev_t USB_Dev;
//uint8_t usbON = 0;

void usb_SetInitMode(uint8_t init)
{
    init_mode = init;
}
//
//static volatile uint8_t tx_succesfull = 1;
//
//#define EP0DATABUF_SIZE                 (64)
//#define LASTADDR_DEFAULT                (STM32ENDPOINTS * 8)
//
//#define STM32ENDPOINTS          8
//#define USB_BTABLE_SIZE         512
//
//// keep all DTOGs and STATs
//#define KEEP_DTOG_STAT(EPnR)            (EPnR & ~(USB_EPnR_STAT_RX|USB_EPnR_STAT_TX|USB_EPnR_DTOG_RX|USB_EPnR_DTOG_TX))
//#define KEEP_DTOG(EPnR)                 (EPnR & ~(USB_EPnR_DTOG_RX|USB_EPnR_DTOG_TX))
//
//// interrupt IN handler
//static void EP1_Handler()
//{
//    uint16_t epstatus = KEEP_DTOG(USB->EP1R);
//
//    if(RX_FLAG(epstatus)) epstatus = (epstatus & ~USB_EPnR_STAT_TX) ^ USB_EPnR_STAT_RX; // set valid RX
//    else
//    {
//        tx_succesfull = 1;
//        epstatus = epstatus & ~(USB_EPnR_STAT_TX | USB_EPnR_STAT_RX);
//    }
//    // clear CTR
//    epstatus = (epstatus & ~(USB_EPnR_CTR_RX | USB_EPnR_CTR_TX));
//    USB->EP1R = epstatus;
//}
//
//
//int EP_Init(uint8_t number, uint8_t type, uint16_t txsz, uint16_t rxsz, void (*func)())
//{
//    if(number >= STM32ENDPOINTS) return 4; // out of configured amount
//    if(txsz > USB_BTABLE_SIZE || rxsz > USB_BTABLE_SIZE) return 1; // buffer too large
//    if(lastaddr + txsz + rxsz >= USB_BTABLE_SIZE) return 2; // out of btable
//    USB->EPnR[number] = (type << 9) | (number & USB_EPnR_EA);
//    USB->EPnR[number] ^= USB_EPnR_STAT_RX | USB_EPnR_STAT_TX_1;
//    if(rxsz & 1 || rxsz > 512) return 3; // wrong rx buffer size
//    uint16_t countrx = 0;
//    if(rxsz < 64) countrx = rxsz / 2;
//    else
//    {
//        if(rxsz & 0x1f) return 3; // should be multiple of 32
//        countrx = 31 + rxsz / 32;
//    }
//    USB_BTABLE->EP[number].USB_ADDR_TX = lastaddr;
//    endpoints[number].tx_buf = (uint16_t *)(USB_BTABLE_BASE + lastaddr * 2);
//    endpoints[number].txbufsz = txsz;
//    lastaddr += txsz;
//    USB_BTABLE->EP[number].USB_COUNT_TX = 0;
//    USB_BTABLE->EP[number].USB_ADDR_RX = lastaddr;
//    endpoints[number].rx_buf = (uint16_t *)(USB_BTABLE_BASE + lastaddr * 2);
//    lastaddr += rxsz;
//    USB_BTABLE->EP[number].USB_COUNT_RX = countrx << 10;
//    endpoints[number].func = func;
//    return 0;
//}

/**
  * @brief This function handles USB FS global interrupt.
  */
void USB_LP_CAN_RX0_IRQHandler(void)
{
    // Catch events by ourself
//    if(init_mode)
//    {
//        if(USB->ISTR & USB_ISTR_RESET)
//        {
//            usbON = 0;
//            debug_println("USB RESET");
//            USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;
//            // Endpoint 0 - CONTROL
//            // ON USB LS size of EP0 may be 8 bytes, but on FS it should be 64 bytes!
//            lastaddr = LASTADDR_DEFAULT;
//            // clear address, leave only enable bit
//            USB->DADDR = USB_DADDR_EF;
//            // state is default - wait for enumeration
////            USB_Dev.USB_Status = USB_STATE_DEFAULT;
//            USB->ISTR = ~USB_ISTR_RESET;
//
//            if(EP_Init(0, EP_TYPE_CONTROL, USB_EP0_BUFSZ, USB_EP0_BUFSZ, EP0_Handler))
//                return;
//        }
//
//        return;
//    }

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

