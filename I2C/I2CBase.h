#pragma once

#include "main.h"

typedef enum I2CAddressingMode {
    I2C_ADDRESSING_MODE_7BIT,
    I2C_ADDRESSING_MODE_10BIT
} I2CAddressingMode;

typedef enum I2CDataDirection {
    I2C_WRITE_TO_SLAVE,
    I2C_READ_FROM_SLAVE
} I2CDataDirection;

typedef enum I2CStatus {
    I2C_OK,
    I2C_START_ERROR,
    I2C_ACK_ERROR,
    I2C_DATA_TX_ERROR,
    I2C_DATA_RX_ERROR,
    I2C_LINE_BUSY_ERROR
} I2CStatus;

static inline uint8_t addWriteDirection7BitI2C(uint32_t address) {
    return (address & (~I2C_OAR1_ADD0));
}

static inline uint8_t addReadDirection7BitI2C(uint32_t address) {
    return (address | I2C_OAR1_ADD0);
}

static inline uint8_t add10BitAddressI2C(uint32_t address) {
    return (address & 0x00FF);
}

static inline uint8_t addWriteDirection10BitHeaderI2C(uint32_t address) {
    return (((address & 0x0300) >> 7) | 0x00F0);
}

static inline uint8_t addReadDirection10BitHeaderI2C(uint32_t address) {
    return (((address & 0x0300) >> 7) | 0x00F1);
}

static inline void write7BitSlaveAddress(I2C_TypeDef *I2Cx, uint32_t address, I2CDataDirection direction) {
    if (direction == I2C_WRITE_TO_SLAVE) {
        I2Cx->DR = addWriteDirection7BitI2C(address);
    } else {
        I2Cx->DR = addReadDirection7BitI2C(address);
    }
}

static inline void write10BitHeaderSlaveAddress(I2C_TypeDef *I2Cx, uint32_t address, I2CDataDirection direction) {
    if (direction == I2C_WRITE_TO_SLAVE) {
        I2Cx->DR = addWriteDirection10BitHeaderI2C(address);
    } else {
        I2Cx->DR = addReadDirection10BitHeaderI2C(address);
    }
}

static inline void write10BitSlaveAddress(I2C_TypeDef *I2Cx, uint32_t address) {
    I2Cx->DR = add10BitAddressI2C(address);
}