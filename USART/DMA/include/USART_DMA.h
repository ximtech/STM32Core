#pragma once

#include <string.h>

#include "main.h"
#include "DMAUtils.h"
#include "Vector.h"

typedef struct USARTData {
    uint32_t stream;
    uint32_t bufferSize;
    char *bufferPointer;
    bool isTransferComplete;
} USARTData;

typedef struct USART_DMA {
    USART_TypeDef *USARTx;
    DMA_TypeDef *DMAx;
    USARTData *rxData;
    USARTData *txData;
} USART_DMA;


USART_DMA *initUSART_DMA(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t txStream, uint32_t rxBufferSize, uint32_t txBufferSize);
USART_DMA *initUSART_DMA_RX(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t rxBufferSize);    // only data receiving
USART_DMA *initUSART_DMA_TX(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t txStream, uint32_t txBufferSize);    // only data transmitting

// Call this subroutines as callbacks at stm32f4xx_it.c, see attached example
void interruptCallbackUSART(USART_TypeDef *USARTx);
void transferCompleteCallbackUSART_DMA(DMA_TypeDef *DMAx, uint32_t stream);

void transmitUSART_DMA(USART_DMA *USARTDmaPointer, char *dataToTransmit, uint32_t length);  // Blocking. After transmit all data will be erased from tx buffer
void receiveUSART_DMA(USART_DMA *USARTDmaPointer, char *dataToReceive, uint32_t length);    // Blocking. After receive data will be copied to provided pointer and then rx buffer will be cleaned

void transmitTxBufferUSART_DMA(USART_DMA *USARTDmaPointer); // Non blocking. Use pointer to access buffers, no data will be erased after tx. Check transfer status with isTransferCompleteUSART_DMA()
void receiveRxBufferUSART_DMA(USART_DMA *USARTDmaPointer);  // Non blocking. Use pointer to access buffers, no data will be erased after tx. Check transfer status with isTransferCompleteUSART_DMA()
bool isTransferCompleteUSART_DMA(USARTData *usartData);

void deleteUSART_DMA(USART_DMA *USARTDmaPointer);