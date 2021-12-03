#pragma once

#include "I2CBase.h"
#include "RingBuffer.h"

#define NUMBER_OF_I2C_INSTANCES 3

typedef struct I2C_IT {
    I2C_TypeDef *I2Cx;
    uint32_t timeout;
    I2CStatus status;
    uint32_t address;
    I2CDataDirection direction;
    I2CAddressingMode addressingMode;
    RingBufferPointer RxBuffer;
    RingBufferPointer TxBuffer;
} I2C_IT;

I2C_IT *initBufferedI2C_IT(I2C_TypeDef *I2Cx, I2CAddressingMode addressingMode, uint32_t rxBufferSize, uint32_t txBufferSize, uint32_t timeout);
void startMasterI2C_IT(BufferedI2C *i2c, uint32_t address, I2CDataDirection direction);
void stopMasterI2C_IT(BufferedI2C *i2c);

void interruptEventCallbackI2C1();// Interrupt callback functions, use by specific I2C number at stm32f4xx_it.c for RX and TX
void interruptEventCallbackI2C2();
void interruptEventCallbackI2C3();

void transmitByteAsMasterI2C_IT(I2C_IT *I2CPointer, uint8_t byte);         // call: startMasterI2C() -> tx, ... -> stopMasterI2C()
uint8_t receiveByteAsMasterI2C_IT(I2C_IT *I2CPointer);                    // call: startMasterI2C() -> rx, ... -> stopMasterI2C()
uint8_t receiveByteAsMasterWithNackI2C_IT(I2C_IT *I2CPointer);

void transmitDataAsMasterI2C_IT(I2C_IT *I2CPointer, uint32_t address, uint8_t *txData, uint16_t size);
void receiveDataAsMasterI2C_IT(I2C_IT *I2CPointer, uint32_t address, uint8_t *data, uint16_t size);

bool isRxBufferEmptyI2C_IT(I2C_IT *I2CPointer);
bool isRxBufferNotEmptyI2C_IT(I2C_IT *I2CPointer);
bool isTxBufferEmptyI2C_IT(I2C_IT *I2CPointer);
bool isTxBufferNotEmptyI2C_IT(I2C_IT *I2CPointer);

void resetRxBufferedI2C_IT(I2C_IT *I2CPointer);
void resetTxBufferedI2C_IT(I2C_IT *I2CPointer);
void deleteI2C_IT(I2C_IT *I2CPointer);