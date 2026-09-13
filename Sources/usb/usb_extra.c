/*
 * usb_extra.c
 *
 *  Created on: 12 сент. 2026 г.
 *      Author: vitaly
 */

#include <string.h>
#include "usb.h"
#include "usb_extra.h"

#define USB_EPnR_STAT_TX_1      0x00000020
#define USB_EPnR_STAT_RX        0x00003000
#define USB_EPnR_EA             0x0000000F
#define USB_EPnR_CTR_TX         0x00000080
#define USB_EPnR_CTR_RX         0x00008000
#define EP_TYPE_INTERRUPT       0x03
#define EP_TYPE_CONTROL         0x01
#define USB_BTABLE_BASE         0x40006000
#define USB_BTABLE              ((USB_BtableDef *)(USB_BTABLE_BASE))

#define STM32ENDPOINTS          8
#define ENDPOINTS_NUM           2
#define USB_BTABLE_SIZE         512
#define LASTADDR_DEFAULT        (STM32ENDPOINTS * 8)

typedef struct
{
    volatile uint32_t USB_ADDR_TX;
    volatile uint32_t USB_COUNT_TX;
    volatile uint32_t USB_ADDR_RX;
    volatile uint32_t USB_COUNT_RX;
} USB_EPDATA_TypeDef;

typedef struct
{
    volatile USB_EPDATA_TypeDef EP[STM32ENDPOINTS];
} USB_BtableDef;

static uint16_t lastaddr = LASTADDR_DEFAULT;

static int EP_Init(uint8_t number, uint8_t type, uint16_t txsz, uint16_t rxsz, void (*func)())
{
    volatile uint16_t* ep[] =
    {
        &USB->EP0R,
        &USB->EP1R
    };

    if(number >= STM32ENDPOINTS)
        return 4; // out of configured amount

    if(txsz > USB_BTABLE_SIZE || rxsz > USB_BTABLE_SIZE)
        return 1; // buffer too large

    if(lastaddr + txsz + rxsz >= USB_BTABLE_SIZE)
        return 2; // out of btable

    *ep[number] = (type << 9) | (number & USB_EPnR_EA);
    *ep[number] ^= USB_EPnR_STAT_RX | USB_EPnR_STAT_TX_1;

    if(rxsz & 1 || rxsz > 512)
        return 3; // wrong rx buffer size

    uint16_t countrx = 0;
    if(rxsz < 64)
        countrx = rxsz / 2;
    else
    {
        if(rxsz & 0x1f)
            return 3; // should be multiple of 32
        countrx = 31 + rxsz / 32;
    }

    USB_BTABLE->EP[number].USB_ADDR_TX = lastaddr;
    lastaddr += txsz;

    USB_BTABLE->EP[number].USB_COUNT_TX = 0;
    USB_BTABLE->EP[number].USB_ADDR_RX = lastaddr;
    lastaddr += rxsz;

    USB_BTABLE->EP[number].USB_COUNT_RX = countrx << 10;

    return 0;
}

void USB_init_workaround()
{
    memset(USB, 0, sizeof(USB_TypeDef));

    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    USB->CNTR   = USB_CNTR_FRES | USB_CNTR_PDWN; // Force USB Reset

    for(uint32_t ctr = 0; ctr < SystemCoreClock / 1000; ++ctr)
        __NOP(); // wait >1ms

    USB->CNTR   = 0;
    USB->BTABLE = 0;
    USB->DADDR  = 0;
    USB->ISTR   = 0;
    USB->CNTR   = USB_CNTR_RESETM | USB_CNTR_WKUPM | USB_CNTR_ESOFM | USB_CNTR_CTRM;

    EP_Init(0, EP_TYPE_CONTROL, 64, 64, NULL);
    EP_Init(1, EP_TYPE_INTERRUPT, 64, 0, NULL);
}
