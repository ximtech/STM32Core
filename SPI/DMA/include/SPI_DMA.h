#pragma once

#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "DMAUtils.h"
#include "Vector.h"

typedef struct SPIData {
    uint32_t stream;
    uint32_t bufferSize;
    union {
        uint8_t *byteBufferPointer;
        uint16_t *halfWordBufferPointer;
    };
    bool isTransferComplete;
} SPIData;

typedef struct SPI_DMA {
    SPI_TypeDef *SPIx;
    DMA_TypeDef *DMAx;
    GPIO_TypeDef *chipSelectPort;
    uint32_t chipSelectPin;
    SPIData *rxData;
    SPIData *txData;
} SPI_DMA;


SPI_DMA *initSPI_DMA(SPI_TypeDef *SPIx, DMA_TypeDef *DMAx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxStream, uint32_t txStream, uint32_t rxBufferSize, uint32_t txBufferSize);
SPI_DMA *initSPI_DMA_RX(SPI_TypeDef *SPIx, DMA_TypeDef *DMAx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxStream, uint32_t rxBufferSize);    // only data receiving
SPI_DMA *initSPI_DMA_TX(SPI_TypeDef *SPIx, DMA_TypeDef *DMAx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t txStream, uint32_t txBufferSize);    // only data transmitting

void transferCompleteCallbackSPI_DMA(DMA_TypeDef *DMAx, uint32_t stream);// Call this subroutine as callback at stm32f4xx_it.c, see attached example

void transmitStringSPI_DMA(SPI_DMA *SPIDmaPointer, char *dataToTransmit, uint32_t length);
void transmitTxBufferSPI_DMA(SPI_DMA *SPIDmaPointer); // Use pointer to access buffers
void receiveRxBufferSPI_DMA(SPI_DMA *SPIDmaPointer);

bool isTransferCompleteSPI_DMA(SPIData *spiData);   // check is data finished receive/transmit

void deleteSPI_DMA(SPI_DMA *SPIDmaPointer);