#pragma once

#include <string.h>
#include <stdlib.h>

#include "I2CBase.h"
#include "DMAUtils.h"
#include "DWT_Delay.h"
#include "Vector.h"

typedef struct I2CData {
    uint32_t stream;
    uint32_t bufferSize;
    uint8_t *bufferPointer;
    bool isTransferComplete;
} I2CData;

typedef struct I2C_DMA {
    I2C_TypeDef *I2Cx;
    DMA_TypeDef *DMAx;
    uint32_t timeout;
    I2CStatus status;
    I2CAddressingMode addressingMode;
    I2CData *rxData;
    I2CData *txData;
} I2C_DMA;

I2C_DMA *initI2C_DMA(I2C_TypeDef *I2Cx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t txStream, uint32_t rxBufferSize, uint32_t txBufferSize, I2CAddressingMode addressingMode, uint32_t timeout);
I2C_DMA *initI2C_DMA_RX(I2C_TypeDef *I2Cx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t rxBufferSize, I2CAddressingMode addressingMode, uint32_t timeout);
I2C_DMA *initI2C_DMA_TX(I2C_TypeDef *I2Cx, DMA_TypeDef *DMAx, uint32_t txStream, uint32_t txBufferSize, I2CAddressingMode addressingMode, uint32_t timeout);

void transferCompleteCallbackI2C_DMA(DMA_TypeDef *DMAx, uint32_t stream);// Call this subroutine as callback at stm32f4xx_it.c, see attached example

void transmitStringI2C_DMA(I2C_DMA *I2CDmaPointer, char *dataToTransmit, uint32_t length, uint32_t address);
void transmitTxBufferI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address); // Use pointer to access buffers
void receiveRxBufferI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address);

bool isTransferCompleteI2C_DMA(I2CData *I2CData);   // check is data finished receive/transmit
bool isDeviceReadyI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address);

void deleteI2C_DMA(I2C_DMA *I2CDmaPointer);