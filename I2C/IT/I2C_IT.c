#include "I2C_IT.h"

#define FIRST_I2C_INSTANCE_INDEX  0
#define SECOND_I2C_INSTANCE_INDEX 1
#define THIRD_I2C_INSTANCE_INDEX  2

static I2C_IT I2CInstanceArray[NUMBER_OF_I2C_INSTANCES];

static I2C_IT *cacheI2CInstance(I2C_IT I2CInstance);
static void interruptCallbackHandler(I2C_IT *I2CPointer);
static void sendSlaveAddress(I2C_IT *I2CPointer);
static void rxInterruptCallbackI2C(I2C_IT *I2CPointer);
static void txInterruptCallbackI2C(I2C_IT *I2CPointer);


I2C_IT *initBufferedI2C_IT(I2C_TypeDef *I2Cx, I2CAddressingMode addressingMode, uint32_t rxBufferSize, uint32_t txBufferSize, uint32_t timeout) {
    I2C_IT I2CInstance = {0};
    I2CInstance.I2Cx = I2Cx;
    I2CInstance.timeout = timeout;
    I2CInstance.addressingMode = addressingMode;
    I2CInstance.RxBuffer = getRingBufferInstance(BYTE_BUFFER, rxBufferSize);
    I2CInstance.TxBuffer = getRingBufferInstance(BYTE_BUFFER, txBufferSize);

    LL_I2C_Enable(I2Cx);
    return cacheI2CInstance(I2CInstance);
}

void startMasterI2C_IT(I2C_IT *I2CPointer, uint32_t address, I2CDataDirection direction) {
    I2CPointer->address = address;
    I2CPointer->direction = direction;

    LL_I2C_EnableIT_EVT(I2CPointer->I2Cx);
    LL_I2C_EnableIT_ERR(I2CPointer->I2Cx);
    LL_I2C_EnableIT_RX(I2CPointer->I2Cx);

    LL_I2C_DisableBitPOS(I2CPointer->I2Cx);
    LL_I2C_AcknowledgeNextData(I2CPointer->I2Cx, LL_I2C_ACK);   // enable address acknowledge
    LL_I2C_GenerateStartCondition(I2CPointer->I2Cx);
}

void stopMasterI2C_IT(I2C_IT *I2CPointer) {
    LL_I2C_GenerateStopCondition(I2CPointer->I2Cx);
    LL_I2C_DisableIT_EVT(I2CPointer->I2Cx);
    LL_I2C_DisableIT_BUF(I2CPointer->I2Cx);
    LL_I2C_DisableIT_ERR(I2CPointer->I2Cx);
    LL_I2C_DisableIT_RX(I2CPointer->I2Cx);
}

void interruptEventCallbackI2C1() {
    I2C_IT *I2CInstancePointer = &I2CInstanceArray[FIRST_I2C_INSTANCE_INDEX];
    interruptCallbackHandler(I2CInstancePointer);
}

void interruptEventCallbackI2C2() {
    I2C_IT *I2CInstancePointer = &I2CInstanceArray[SECOND_I2C_INSTANCE_INDEX];
    interruptCallbackHandler(I2CInstancePointer);
}

void interruptEventCallbackI2C3() {
    I2C_IT *I2CInstancePointer = &I2CInstanceArray[THIRD_I2C_INSTANCE_INDEX];
    interruptCallbackHandler(I2CInstancePointer);
}

void transmitByteAsMasterI2C_IT(I2C_IT *I2CPointer, uint8_t byte) {
    while (isFull(I2CPointer->TxBuffer));
    addByte(I2CPointer->TxBuffer, byte);
    LL_I2C_EnableIT_TX(I2CPointer->I2Cx);
}

uint8_t receiveByteAsMasterI2C_IT(I2C_IT *I2CPointer) {
    uint8_t byte = 0;
    getByte(I2CPointer->RxBuffer, &byte);
    return byte;
}

uint8_t receiveByteAsMasterWithNackI2C_IT(I2C_IT *I2CPointer) {
    LL_I2C_AcknowledgeNextData(I2CPointer->I2Cx, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2CPointer->I2Cx);

    LL_I2C_DisableIT_EVT(I2CPointer->I2Cx);
    LL_I2C_DisableIT_BUF(I2CPointer->I2Cx);
    LL_I2C_DisableIT_ERR(I2CPointer->I2Cx);
    LL_I2C_DisableIT_RX(I2CPointer->I2Cx);
    return LL_I2C_ReceiveData8(I2CPointer->I2Cx);
}

void transmitDataAsMasterI2C_IT(I2C_IT *I2CPointer, uint32_t address, uint8_t *txData, uint16_t size) {
    while (!LL_I2C_IsActiveFlag_TXE(I2CPointer->I2Cx));
    for (int i = 0; i < size; i++) {
        if (isNotFull(I2CPointer->TxBuffer)) {
            addByte(I2CPointer->TxBuffer, txData[i]);
        } else {
            LL_I2C_EnableIT_TX(I2CPointer->I2Cx);
            while (isFull(I2CPointer->TxBuffer));
            addByte(I2CPointer->TxBuffer, txData[i]);
        }
    }
    startMasterI2C_IT(I2CPointer, address, I2C_WRITE_TO_SLAVE);
    LL_I2C_EnableIT_TX(I2CPointer->I2Cx);
}

void receiveDataAsMasterI2C_IT(I2C_IT *I2CPointer, uint32_t address, uint8_t *rxData, uint16_t size) {
    startMasterI2C_IT(I2CPointer, address, I2C_READ_FROM_SLAVE);
    while (LL_I2C_IsActiveFlag_RXNE(I2CPointer->I2Cx) == RESET);
    uint8_t byte = 0;
    for (uint16_t i = 0; getByte(I2CPointer->RxBuffer, &byte) && (i < size); i++) {
        rxData[i] = byte;
    }
    stopMasterI2C_IT(I2CPointer);
}

bool isRxBufferEmptyI2C_IT(I2C_IT *I2CPointer) {
    return isEmpty(I2CPointer->RxBuffer);
}

bool isRxBufferNotEmptyI2C_IT(I2C_IT *I2CPointer) {
    return isNotEmpty(I2CPointer->RxBuffer);
}

bool isTxBufferEmptyI2C_IT(I2C_IT *I2CPointer) {
    return isEmpty(I2CPointer->TxBuffer);
}

bool isTxBufferNotEmptyI2C_IT(I2C_IT *I2CPointer) {
    return isNotEmpty(I2CPointer->TxBuffer);
}

void resetRxBufferedI2C_IT(I2C_IT *I2CPointer) {
    reset(I2CPointer->RxBuffer);
}

void resetTxBufferedI2C_IT(I2C_IT *I2CPointer) {
    reset(I2CPointer->TxBuffer);
}

void deleteI2C_IT(I2C_IT *I2CPointer) {
    delete(I2CPointer->RxBuffer);
    delete(I2CPointer->TxBuffer);
    free(I2CPointer);
}

static I2C_IT *cacheI2CInstance(I2C_IT I2CInstance) {
    if (I2CInstance.I2Cx == I2C1) {
        I2CInstanceArray[FIRST_I2C_INSTANCE_INDEX] = I2CInstance;
        return &I2CInstanceArray[FIRST_I2C_INSTANCE_INDEX];
    } else if (I2CInstance.I2Cx == I2C2) {
        I2CInstanceArray[SECOND_I2C_INSTANCE_INDEX] = I2CInstance;
        return &I2CInstanceArray[SECOND_I2C_INSTANCE_INDEX];
    } else if (I2CInstance.I2Cx == I2C3) {
        I2CInstanceArray[THIRD_I2C_INSTANCE_INDEX] = I2CInstance;
        return &I2CInstanceArray[THIRD_I2C_INSTANCE_INDEX];
    }
    return NULL;
}

static void interruptCallbackHandler(I2C_IT *I2CPointer) {
    if (LL_I2C_IsActiveFlag_RXNE(I2CPointer->I2Cx)) {
        rxInterruptCallbackI2C(I2CPointer);
    } else if (LL_I2C_IsActiveFlag_TXE(I2CPointer->I2Cx)) {
        txInterruptCallbackI2C(I2CPointer);
    } else if (LL_I2C_IsActiveFlag_AF(I2CPointer->I2Cx)) {
        LL_I2C_ClearFlag_AF(I2CPointer->I2Cx);
        I2CPointer->status = I2C_DATA_TX_ERROR;
    } else if (LL_I2C_IsActiveFlag_BERR(I2CPointer->I2Cx)) {
        LL_I2C_ClearFlag_BERR(I2CPointer->I2Cx);
    } else if (LL_I2C_IsActiveFlag_BUSY(I2CPointer->I2Cx)) {
        if (LL_I2C_IsActiveFlag_SB(I2CPointer->I2Cx)) {
            sendSlaveAddress(I2CPointer);
        }
    }
}

static void sendSlaveAddress(I2C_IT *I2CPointer) {
    if (I2CPointer->addressingMode == I2C_ADDRESSING_MODE_7BIT) {
        LL_I2C_ClearFlag_ADDR(I2CPointer->I2Cx);
        write7BitSlaveAddress(I2CPointer->I2Cx, I2CPointer->address, I2CPointer->direction);
    } else {
        LL_I2C_ClearFlag_ADDR(I2CPointer->I2Cx);
        if (LL_I2C_IsActiveFlag_ADD10(I2CPointer->I2Cx)) {
            write10BitSlaveAddress(I2CPointer->I2Cx, I2CPointer->address);     // Send slave address
        } else {
            write10BitHeaderSlaveAddress(I2CPointer->I2Cx, I2CPointer->direction, I2CPointer->address);
        }
    }
}

static void rxInterruptCallbackI2C(I2C_IT *I2CPointer) {
    if (isNotFull(I2CPointer->RxBuffer)) {        // when buffer overflow, doesn't overwrite non read data
        uint8_t byte = LL_I2C_ReceiveData8(I2CPointer->I2Cx);
        addByte(I2CPointer->RxBuffer, byte);
    }
}

static void txInterruptCallbackI2C(I2C_IT *I2CPointer) {
    uint8_t byte = 0;
    if (getByte(I2CPointer->TxBuffer, &byte)) {
        LL_I2C_TransmitData8(I2CPointer->I2Cx, byte);
    } else {
        LL_I2C_DisableIT_TX(I2CPointer->I2Cx);// tx buffer empty, disable interrupt
    }
}