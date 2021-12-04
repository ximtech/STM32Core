#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "main.h"
#include "RingBuffer.h"

typedef struct USART_IT {
    USART_TypeDef *USARTx;
    RingBufferPointer RxBuffer;
    RingBufferPointer TxBuffer;
} USART_IT;

USART_IT *initBufferedUSART_IT(USART_TypeDef *USARTx, uint32_t rxBufferSize, uint32_t txBufferSize);

void interruptCallbackUSART1();// Interrupt callback functions, use by specific UART number at stm32f4xx_it.c
void interruptCallbackUSART2();
void interruptCallbackUSART6();

void sendByteUSART_IT(USART_IT *USARTPointer, uint8_t byte);
void sendStringUSART_IT(USART_IT *USARTPointer, const char *string);
void sendFormattedStringUSART_IT(USART_IT *USARTPointer, uint16_t bufferLength, char *format, ...);

uint8_t readByteUSART_IT(USART_IT *USARTPointer);
void readStringUSART_IT(USART_IT *USARTPointer, char *charArray);
void readStringForLengthUSART_IT(USART_IT *USARTPointer, char *charArray, uint32_t length);
void readStringUntilStopCharUSART_IT(USART_IT *USARTPointer, char *charArray, char stopChar);

void deleteUSART_IT(USART_IT *USARTPointer);


// Helper functions
static inline bool isRxBufferEmptyUSART_IT(USART_IT *USARTPointer) {
    return isRingBufferEmpty(USARTPointer->RxBuffer);    // buffer empty if no bytes received
}

static inline bool isRxBufferNotEmptyUSART_IT(USART_IT *USARTPointer) {
    return isRingBufferNotEmpty(USARTPointer->RxBuffer);
}

static inline bool isRxBufferFullUSART_IT(USART_IT *USARTPointer) {
    return isRingBufferFull(USARTPointer->RxBuffer);
}

static inline void resetRxBufferUSART_IT(USART_IT *USARTPointer) {
    ringBufferReset(USARTPointer->RxBuffer);
}

static inline void resetTxBufferUSART_IT(USART_IT *USARTPointer) {
    ringBufferReset(USARTPointer->TxBuffer);
}