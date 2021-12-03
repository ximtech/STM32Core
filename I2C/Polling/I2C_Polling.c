#include "I2C_Polling.h"

static I2CStatus sendSlaveAddress(I2C_Polling *i2c, uint32_t address, I2CDataDirection direction);
static bool loopWhileCheckFunctionStatus(I2C_Polling *i2c, uint32_t (*checkFunctionPointer)(I2C_TypeDef *), FlagStatus status);
static I2CStatus waitTransmissionStatus(I2C_Polling *i2c);
static I2CStatus waitReceptionStatus(I2C_Polling *i2c);


I2C_Polling initI2C(I2C_TypeDef *I2Cx, I2CAddressingMode addressingMode, uint32_t timeout) {
    I2C_Polling i2c = {0};
    i2c.I2Cx = I2Cx;
    i2c.timeout = timeout;
    i2c.addressingMode = addressingMode;
    dwtDelayInit();
    LL_I2C_Enable(I2Cx);
    return i2c;
}

I2CStatus startAsMasterI2C(I2C_Polling *i2c, uint32_t address, I2CDataDirection direction) {
    LL_I2C_DisableBitPOS(i2c->I2Cx);
    LL_I2C_AcknowledgeNextData(i2c->I2Cx, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(i2c->I2Cx);

    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_SB, RESET)) {
        stopAsMasterI2C(i2c);
        return I2C_START_ERROR;
    }
    return sendSlaveAddress(i2c, address, direction);
}

I2CStatus stopAsMasterI2C(I2C_Polling *i2c) {
    LL_I2C_GenerateStopCondition(i2c->I2Cx);
    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_BUSY, SET)) {
        return I2C_LINE_BUSY_ERROR;
    }
    return I2C_OK;
}

I2CStatus transmitByteAsMasterI2C(I2C_Polling *i2c, uint8_t byte) {
    if (waitTransmissionStatus(i2c) != I2C_OK) {
        return I2C_DATA_TX_ERROR;
    }

    LL_I2C_TransmitData8(i2c->I2Cx, byte);
    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_BTF, RESET)) { // check that byte is send
        return I2C_DATA_TX_ERROR;
    }
    return I2C_OK;
}

I2CStatus receiveByteAsMasterI2C(I2C_Polling *i2c, uint8_t *receiveByte) {
    if (waitReceptionStatus(i2c) != I2C_OK) {
        return I2C_DATA_RX_ERROR;
    }
    *receiveByte = LL_I2C_ReceiveData8(i2c->I2Cx);
    return I2C_OK;
}

I2CStatus receiveByteAsMasterWithNackI2C(I2C_Polling *i2c, uint8_t *receiveByte) {
    LL_I2C_AcknowledgeNextData(i2c->I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(i2c->I2Cx);
    if (waitReceptionStatus(i2c) != I2C_OK) {
        return I2C_DATA_RX_ERROR;
    }
    *receiveByte = LL_I2C_ReceiveData8(i2c->I2Cx);
    return I2C_OK;
}

I2CStatus transmitDataAsMasterI2C(I2C_Polling *i2c, uint32_t address, uint8_t *data, uint16_t size) {
    startAsMasterI2C(i2c, address, I2C_WRITE_TO_SLAVE);
    for (uint16_t i = 0; i < size; i++) {
        if (transmitByteAsMasterI2C(i2c, data[i]) != I2C_OK) {
            return I2C_DATA_TX_ERROR;
        }
    }
    stopAsMasterI2C(i2c);
    return I2C_OK;
}

I2CStatus receiveDataAsMasterI2C(I2C_Polling *i2c, uint32_t address, uint8_t *data, uint16_t size) {
    startAsMasterI2C(i2c, address, I2C_READ_FROM_SLAVE);
    for (uint16_t i = 0; i < size; i++) {
        if (receiveByteAsMasterI2C(i2c, &data[i]) != I2C_OK) {
            return I2C_DATA_RX_ERROR;
        }
    }
    stopAsMasterI2C(i2c);
    return I2C_OK;
}

I2CStatus transmitDataAsSlaveI2C(I2C_Polling *i2c, uint8_t *data, uint16_t size) {
    LL_I2C_AcknowledgeNextData(i2c->I2Cx, LL_I2C_ACK);   // enable address acknowledge
    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_ADDR, RESET)) {  // wait for address matched flag (slave mode)
        return I2C_DATA_TX_ERROR;
    }
    LL_I2C_ClearFlag_ADDR(i2c->I2Cx);

    if (i2c->addressingMode == I2C_ADDRESSING_MODE_10BIT) {
        if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_ADDR, RESET)) {  // wait for second address matched flag
            return I2C_DATA_TX_ERROR;
        }
    }

    for (uint16_t i = 0; i < size; i++) {
        if (waitTransmissionStatus(i2c) != I2C_OK) {
            CLEAR_BIT(i2c->I2Cx->CR1, I2C_CR1_ACK);   // disable address acknowledge
            return I2C_DATA_TX_ERROR;
        }
        LL_I2C_TransmitData8(i2c->I2Cx, data[i]);
    }

    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_AF, RESET)) { // check that no acknowledge failure
        return I2C_DATA_TX_ERROR;
    }
    LL_I2C_ClearFlag_AF(i2c->I2Cx);
    CLEAR_BIT(i2c->I2Cx->CR1, I2C_CR1_ACK);   // disable address acknowledge
    return I2C_OK;
}

I2CStatus receiveDataAsSlaveI2C(I2C_Polling *i2c, uint8_t *data, uint16_t size) {
    LL_I2C_AcknowledgeNextData(i2c->I2Cx, LL_I2C_ACK);   // enable address acknowledge
    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_ADDR, RESET)) {  // wait for address matched flag (slave mode)
        return I2C_DATA_TX_ERROR;
    }
    LL_I2C_ClearFlag_ADDR(i2c->I2Cx);

    for (uint16_t i = 0; i < size; i++) {
        if (waitReceptionStatus(i2c) != I2C_OK) {
            CLEAR_BIT(i2c->I2Cx->CR1, I2C_CR1_ACK);   // disable address acknowledge
            return I2C_DATA_RX_ERROR;
        }
        data[i] = LL_I2C_ReceiveData8(i2c->I2Cx);
    }

    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_STOP, SET)) {  // wait until STOP flag is set
        CLEAR_BIT(i2c->I2Cx->CR1, I2C_CR1_ACK);   // disable address acknowledge
        return I2C_DATA_TX_ERROR;
    }
    LL_I2C_ClearFlag_STOP(i2c->I2Cx);
    CLEAR_BIT(i2c->I2Cx->CR1, I2C_CR1_ACK);
    return I2C_OK;
}

I2CStatus isDeviceReady(I2C_Polling *i2c, uint32_t address) {
    I2CStatus status = startAsMasterI2C(i2c, address, I2C_WRITE_TO_SLAVE);
    stopAsMasterI2C(i2c);
    return status;
}

static I2CStatus sendSlaveAddress(I2C_Polling *i2c, uint32_t address, I2CDataDirection direction) {
    LL_I2C_ClearFlag_ADDR(i2c->I2Cx);

    if (i2c->addressingMode == I2C_ADDRESSING_MODE_7BIT) {  // 0: 7-bit slave address (10-bit address not acknowledged)
        write7BitSlaveAddress(i2c->I2Cx, address, direction);
    } else {
        write10BitHeaderSlaveAddress(i2c->I2Cx, address, direction);

        if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_ADD10, RESET)) {// Wait until ADD10 flag is set
            LL_I2C_ClearFlag_AF(i2c->I2Cx);
            stopAsMasterI2C(i2c);
            return I2C_ACK_ERROR;
        }
        write10BitSlaveAddress(i2c->I2Cx, address);   // Send slave address
    }

    if (!loopWhileCheckFunctionStatus(i2c, LL_I2C_IsActiveFlag_ADDR, RESET)) {// Wait until ADDR flag is set
        LL_I2C_ClearFlag_AF(i2c->I2Cx);
        stopAsMasterI2C(i2c);
        return I2C_ACK_ERROR;
    }
    LL_I2C_ClearFlag_ADDR(i2c->I2Cx);
    return I2C_OK;
}

static bool loopWhileCheckFunctionStatus(I2C_Polling *i2c, uint32_t (*checkFunctionPointer)(I2C_TypeDef *), FlagStatus status) {
    uint32_t startTime = currentMilliSeconds();
    while (checkFunctionPointer(i2c->I2Cx) == status) {
        if ((currentMilliSeconds() - startTime) >= i2c->timeout) {
            return false;
        }
    }
    return true;
}

static I2CStatus waitTransmissionStatus(I2C_Polling *i2c) {
    uint32_t startTime = currentMilliSeconds();
    while (LL_I2C_IsActiveFlag_TXE(i2c->I2Cx) == RESET) {
        if ((currentMilliSeconds() - startTime) >= i2c->timeout) {
            return I2C_DATA_TX_ERROR;
        }

        if (LL_I2C_IsActiveFlag_AF(i2c->I2Cx)) {
            LL_I2C_ClearFlag_AF(i2c->I2Cx);
            return I2C_DATA_TX_ERROR;
        }
    }
    return I2C_OK;
}

static I2CStatus waitReceptionStatus(I2C_Polling *i2c) {
    uint32_t startTime = currentMilliSeconds();
    while (LL_I2C_IsActiveFlag_RXNE(i2c->I2Cx) == RESET) {
        if ((currentMilliSeconds() - startTime) >= i2c->timeout) {
            return I2C_DATA_RX_ERROR;
        }

        if (LL_I2C_IsActiveFlag_STOP(i2c->I2Cx)) {
            LL_I2C_ClearFlag_STOP(i2c->I2Cx);
            return I2C_DATA_RX_ERROR;
        }
    }
    return I2C_OK;
}