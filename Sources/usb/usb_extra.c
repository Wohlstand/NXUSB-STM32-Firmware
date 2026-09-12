/*
 * usb_extra.c
 *
 *  Created on: 12 сент. 2026 г.
 *      Author: vitaly
 */


#include "usb.h"
//#include "usb_defs.h"
#include "usb_extra.h"

#define USB_EPnR_STAT_TX_1      0x00000020
#define USB_EPnR_STAT_RX        0x00003000
#define USB_EPnR_EA             0x0000000F
#define USB_EPnR_CTR_TX         0x00000080
#define USB_EPnR_CTR_RX         0x00008000
#define EP_TYPE_INTERRUPT               0x03

//#define  USB_ISTR_RESET                      ((uint16_t)0x0400)            /*!< USB RESET request */

void USB_setup()
{
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    USB->CNTR   = USB_CNTR_FRES; // Force USB Reset

    for(uint32_t ctr = 0; ctr < 72000; ++ctr)
        __NOP(); // wait >1ms

    USB->CNTR   = 0;
    USB->BTABLE = 0;
    USB->DADDR  = 0;
    USB->ISTR   = 0;
    USB->CNTR   = USB_CNTR_RESETM | USB_CNTR_WKUPM; // allow only wakeup & reset interrupts
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);

//    USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;
    // clear address, leave only enable bit
//    USB->DADDR = USB_DADDR_EF;
//    USB->ISTR = ~((uint16_t)0x0400);

//    USB->EP1R = (EP_TYPE_INTERRUPT << 9) | (1 & USB_EPnR_EA);
//    USB->EP1R ^= USB_EPnR_STAT_RX | USB_EPnR_STAT_TX_1;
//    USB->EP1R = 0;
}
