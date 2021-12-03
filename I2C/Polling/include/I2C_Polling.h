#pragma once

#include <stdbool.h>
#include "I2CBase.h"
#include "DWT_Delay.h"

typedef struct I2C_Polling {
    I2C_TypeDef *I2Cx;
    uint32_t timeout;
    I2CAddressingMode addressingMode;
} I2C_Polling;

I2C_Polling initI2C(I2C_TypeDef *I2Cx, I2CAddressingMode addressingMode, uint32_t timeout);

I2CStatus startAsMasterI2C(I2C_Polling *i2c, uint32_t address, I2CDataDirection direction);
I2CStatus stopAsMasterI2C(I2C_Polling *i2c);

I2CStatus transmitByteAsMasterI2C(I2C_Polling *i2c, uint8_t byte);         // call: startAsMasterI2C() -> tx, ... -> stopAsMasterI2C()
I2CStatus receiveByteAsMasterI2C(I2C_Polling *i2c, uint8_t *receiveByte); // call: startAsMasterI2C() -> rx, ... -> stopAsMasterI2C()
I2CStatus receiveByteAsMasterWithNackI2C(I2C_Polling *i2c, uint8_t *receiveByte);

I2CStatus transmitDataAsMasterI2C(I2C_Polling *i2c, uint32_t address, uint8_t *data, uint16_t size);
I2CStatus receiveDataAsMasterI2C(I2C_Polling *i2c, uint32_t address, uint8_t *data, uint16_t size);

I2CStatus transmitDataAsSlaveI2C(I2C_Polling *i2c, uint8_t *data, uint16_t size);
I2CStatus receiveDataAsSlaveI2C(I2C_Polling *i2c, uint8_t *data, uint16_t size);

I2CStatus isDeviceReady(I2C_Polling *i2c, uint32_t address);