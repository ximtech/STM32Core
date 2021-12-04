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

SPI_IT *initSPI_IT(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxBufferSize, uint32_t txBufferSize);

void interruptCallbackSPI1();// Interrupt callback functions, use by specific SPI number at stm32fXxx_it.c for RX and TX, auto data type selection
void interruptCallbackSPI2();
void interruptCallbackSPI3();

void chipSelectSetSPI_IT(SPI_IT *SPIPointer);
void chipSelectResetSPI_IT(SPI_IT *SPIPointer);

void transmitDataSPI_IT(SPI_IT *SPIPointer, RingBufferDataType data);  // manual CS on/off for data TX RX
RingBufferDataType receiveDataSPI_IT(SPI_IT *SPIPointer);

void transmitMultipleDataSPI_IT(SPI_IT *SPIPointer, const RingBufferDataType *txData, uint16_t length);// auto CS on/off for data TX RX
void receiveMultipleDataSPI_IT(SPI_IT *SPIPointer, RingBufferDataType *rxData, uint16_t length);

bool isRxBufferEmptySPI(SPI_IT *SPIPointer);
bool isRxBufferNotEmptySPI(SPI_IT *SPIPointer);
bool isTxBufferEmptySPI(SPI_IT *SPIPointer);
bool isTxBufferNotEmptySPI(SPI_IT *SPIPointer);

void resetRxSPI_IT(SPI_IT *SPIPointer);
void resetTxSPI_IT(SPI_IT *SPIPointer);
void deleteSPI_IT(SPI_IT *SPIPointer);