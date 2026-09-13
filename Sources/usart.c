/*
 * usart.c
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#include <string.h>
#include "usart.h"
#include "debug_uart.h"

UART_HandleTypeDef huart1;
//DMA_HandleTypeDef hdma_tx;
//DMA_HandleTypeDef hdma_rx;

void MX_USART1_UART_Init(void)
{
    uint32_t tmout = 16000000;
//    __HAL_RCC_USART1_CLK_ENABLE();
    memset(&huart1, 0, sizeof(UART_HandleTypeDef));

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
    NVIC_SetPriority(DMA1_Channel4_IRQn, 3);

    USART1->BRR = 72000000 / 115200;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    while(!(USART1->SR & USART_SR_TC))
    {
        if(--tmout == 0) break;   // polling idle frame Transmission
    }

    USART1->SR = 0; // clear flags
    USART1->CR1 |= USART_CR1_RXNEIE;
    USART1->CR3 = USART_CR3_DMAT;

    NVIC_SetPriority(USART1_IRQn, 0);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }


//    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
//    {
//        Error_Handler();
//    }
//
//    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
//    {
//        Error_Handler();
//    }
//
//    if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
//    {
//        Error_Handler();
//    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    if(uartHandle->Instance == USART1)
    {
//        /* Configure the DMA handler for Transmission process */
//        hdma_tx.Instance                 = DMA1_Channel4;
//        hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
//        hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
//        hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
//        hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
//        hdma_tx.Init.Mode                = DMA_NORMAL;
//        hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
//
//        HAL_DMA_Init(&hdma_tx);
//
//        /* Associate the initialised DMA handle to the UART handle */
//        __HAL_LINKDMA(&huart1, hdmatx, hdma_tx);
//
//        /* Configure the DMA handler for reception process */
//        hdma_rx.Instance                 = DMA1_Channel5;
//        hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
//        hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
//        hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
//        hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
//        hdma_rx.Init.Mode                = DMA_NORMAL;
//        hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
//
//        HAL_DMA_Init(&hdma_rx);
//
//        /* Associate the initialised DMA handle to the the UART handle */
//        __HAL_LINKDMA(&huart1, hdmarx, hdma_rx);
//
//
//        /* NVIC configuration for DMA transfer complete interrupt (USARTx_TX) */
//        HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 1);
//        HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
//
//        /* NVIC configuration for DMA transfer complete interrupt (USARTx_RX) */
//        HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
//        HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
//
//        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
//
//        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
//        __HAL_RCC_DMA1_CLK_ENABLE();
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    (void)uartHandle;
    __HAL_RCC_USART1_CLK_DISABLE();
//    if(uartHandle->Instance == USART1)
//    {
////        /* Peripheral clock disable */
////        __HAL_RCC_USART1_CLK_DISABLE();
////        __HAL_RCC_DMA1_CLK_DISABLE();
////
////        __HAL_RCC_USART1_FORCE_RESET();
////        __HAL_RCC_USART1_RELEASE_RESET();
////
////        /**USART1 GPIO Configuration
////        PB14     ------> USART1_TX
////        PB15     ------> USART1_RX
////        */
////        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
////        /* USART1 interrupt Deinit */
////        HAL_NVIC_DisableIRQ(USART1_IRQn);
////
////        /* De-Initialize the DMA Channel associated to transmission process */
////        HAL_DMA_DeInit(&hdma_tx);
////        /* De-Initialize the DMA Channel associated to reception process */
////        HAL_DMA_DeInit(&hdma_rx);
////
////        HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);
////        HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
//    }
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

//void DMA1_Channel4_IRQHandler(void)
//{
//    HAL_DMA_IRQHandler(huart1.hdmatx);
//}
//
//void DMA1_Channel5_IRQHandler(void)
//{
//    HAL_DMA_IRQHandler(huart1.hdmarx);
//}
