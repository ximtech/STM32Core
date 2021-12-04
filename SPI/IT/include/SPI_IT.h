#pragma once

#include "main.h"
#include "RingBuffer.h"

typedef struct SPI_IT {
    SPI_TypeDef *SPIx;
    GPIO_TypeDef *chipSelectPort;
    uint32_t chipSelectPin;
    RingBufferPointer RxBuffer;
    RingBufferPointer TxBuffer;
} SPI_IT;

SPI_IT *initBufferedSPI(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxBufferSize, uint32_t txBufferSize);

void interruptCallbackSPI1();// Interrupt callback functions, use by specific SPI number at stm32f4xx_it.c for RX and TX, auto data type selection
void interruptCallbackSPI2();
void interruptCallbackSPI3();

void chipSelectSetBufferedSPI(SPI_IT *SPIPointer);
void chipSelectResetBufferedSPI(SPI_IT *SPIPointer);

void transmit8BitsBufferedSPI(SPI_IT *SPIPointer, uint8_t byte);  // manual CS on/off for data TX RX
void transmit16BitsBufferedSPI(SPI_IT *SPIPointer, uint16_t halfWord);

uint8_t receive8BitsBufferedSPI(SPI_IT *SPIPointer);
uint16_t receive16BitsBufferedSPI(SPI_IT *SPIPointer);

void transmit8BitDataBufferedSPI(SPI_IT *SPIPointer, const uint8_t *txData, uint16_t length);// auto CS on/off for data TX RX
void transmit16BitDataBufferedSPI(SPI_IT *SPIPointer, const uint16_t *txData, uint16_t length);

void receive8bitDataBufferedSPI(SPI_IT *SPIPointer, uint8_t *rxData, uint16_t length);
void receive16bitDataBufferedSPI(SPI_IT *SPIPointer, uint16_t *rxData, uint16_t length);

bool isRxBufferEmptySPI(SPI_IT *SPIPointer);
bool isRxBufferNotEmptySPI(SPI_IT *SPIPointer);
bool isTxBufferEmptySPI(SPI_IT *SPIPointer);
bool isTxBufferNotEmptySPI(SPI_IT *SPIPointer);

void resetRxBufferedSPI(SPI_IT *SPIPointer);
void resetTxBufferedSPI(SPI_IT *SPIPointer);
void deleteSPI(SPI_IT *SPIPointer);