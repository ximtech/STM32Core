#include "I2C_DMA.h"

static I2C_DMA *I2C_DMAInstanceArray[NUMBER_OF_I2C_INSTANCES] = {[0 ... NUMBER_OF_I2C_INSTANCES - 1] = NULL};

static I2C_DMA *cacheI2CInstance(I2C_DMA *I2CDmaInstance);
static I2CStatus startAsMasterI2C(I2C_DMA *I2CDmaInstance, uint32_t address, I2CDataDirection direction);
static void stopAsMasterI2C(I2C_DMA *I2CDmaInstance);
static I2CStatus sendSlaveAddress(I2C_DMA *I2CDmaInstance, uint32_t address, I2CDataDirection direction);
static bool loopWhileCheckFunctionStatusReset(I2C_DMA *I2CDmaInstance, uint32_t (*checkFunctionPointer)(I2C_TypeDef *));


I2C_DMA *initI2C_DMA(I2C_TypeDef *I2Cx,
                     DMA_TypeDef *DMAx,
                     uint32_t rxStream,
                     uint32_t txStream,
                     uint32_t rxBufferSize,
                     uint32_t txBufferSize,
                     I2CAddressingMode addressingMode,
                     uint32_t timeout) {
    I2C_DMA *I2CDmaInstance = malloc(sizeof(I2C_DMA));
    if (I2CDmaInstance == NULL) return NULL;
    I2CDmaInstance->I2Cx = I2Cx;
    I2CDmaInstance->DMAx = DMAx;
    I2CDmaInstance->timeout = timeout;
    I2CDmaInstance->status = I2C_OK;
    I2CDmaInstance->addressingMode = addressingMode;
    I2CDmaInstance->rxData = NULL;
    I2CDmaInstance->txData = NULL;

    if (rxBufferSize > 0) {
        I2CData *rxData = malloc(sizeof(I2CData));
        uint8_t *rxBufferPointer = malloc(sizeof(uint8_t) * rxBufferSize);
        if (rxData == NULL || rxBufferPointer == NULL) {
            deleteI2C_DMA(I2CDmaInstance);
            return NULL;
        }
        I2CDmaInstance->rxData = rxData;
        memset(rxBufferPointer, 0, rxBufferSize);

        enableDMAStream(DMAx, rxStream);
        LL_I2C_EnableDMAReq_RX(I2Cx);

        uint32_t dataRegisterAddress = LL_I2C_DMA_GetRegAddr(I2Cx);
        uint32_t rxDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, rxStream);
        LL_DMA_ConfigAddresses(DMAx, rxStream, dataRegisterAddress, (uint32_t) rxData->bufferPointer, rxDataTransferDirection);

        I2CDmaInstance->rxData->stream = rxStream;
        I2CDmaInstance->rxData->bufferSize = rxBufferSize;
        I2CDmaInstance->rxData->bufferPointer = rxBufferPointer;
        I2CDmaInstance->rxData->isTransferComplete = false;
    }

    if (txBufferSize > 0) {
        I2CData *txData = malloc(sizeof(I2CData));
        uint8_t *txBufferPointer = malloc(sizeof(uint8_t) * txBufferSize);
        if (txData == NULL || txBufferPointer == NULL) {
            deleteI2C_DMA(I2CDmaInstance);
            return NULL;
        }
        I2CDmaInstance->txData = txData;
        memset(txBufferPointer, 0, rxBufferSize);

        enableDMAStream(DMAx, txStream);
        LL_I2C_EnableDMAReq_TX(I2Cx);

        uint32_t dataRegisterAddress = LL_I2C_DMA_GetRegAddr(I2Cx);
        uint32_t txDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, txStream);
        LL_DMA_ConfigAddresses(DMAx, txStream, (uint32_t) txBufferPointer, dataRegisterAddress, txDataTransferDirection);

        I2CDmaInstance->txData->stream = txStream;
        I2CDmaInstance->txData->bufferSize = txBufferSize;
        I2CDmaInstance->txData->bufferPointer = txBufferPointer;
        I2CDmaInstance->txData->isTransferComplete = false;
    }

    LL_I2C_Enable(I2Cx);
    return cacheI2CInstance(I2CDmaInstance);
}

I2C_DMA *initI2C_DMA_RX(I2C_TypeDef *I2Cx,
                        DMA_TypeDef *DMAx,
                        uint32_t rxStream,
                        uint32_t rxBufferSize,
                        I2CAddressingMode addressingMode,
                        uint32_t timeout) {
    return initI2C_DMA(I2Cx, DMAx, rxStream, 0, rxBufferSize, 0, addressingMode, timeout);
}

I2C_DMA *initI2C_DMA_TX(I2C_TypeDef *I2Cx,
                        DMA_TypeDef *DMAx,
                        uint32_t txStream,
                        uint32_t txBufferSize,
                        I2CAddressingMode addressingMode,
                        uint32_t timeout) {
    return initI2C_DMA(I2Cx, DMAx, 0, txStream, 0, txBufferSize, addressingMode, timeout);
}

void transferCompleteCallbackI2C_DMA(DMA_TypeDef *DMAx, uint32_t stream) {
    for (uint8_t i = 0; i < NUMBER_OF_I2C_INSTANCES; i++) {
        I2C_DMA *I2CDmaPointer = I2C_DMAInstanceArray[i];
        if (I2CDmaPointer->DMAx == DMAx && I2CDmaPointer->rxData != NULL && I2CDmaPointer->rxData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(DMAx, stream)) {
                disableTransferCompleteInterruptDMA(DMAx, stream);
                I2CDmaPointer->rxData->isTransferComplete = true;
                I2CDmaPointer->status = I2C_OK;
            } else if (isTransferErrorInterruptEnabledDMA(DMAx, stream)) {
                disableTransferErrorInterruptDMA(DMAx, stream);
                I2CDmaPointer->status = I2C_DATA_RX_ERROR;
            }
            stopAsMasterI2C(I2CDmaPointer);

        } else if (I2CDmaPointer->DMAx == DMAx && I2CDmaPointer->txData != NULL && I2CDmaPointer->txData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(DMAx, stream)) {
                disableTransferCompleteInterruptDMA(DMAx, stream);
                I2CDmaPointer->txData->isTransferComplete = true;
                I2CDmaPointer->status = I2C_OK;
            } else if (isTransferErrorInterruptEnabledDMA(DMAx, stream)) {
                disableTransferErrorInterruptDMA(DMAx, stream);
                I2CDmaPointer->status = I2C_DATA_TX_ERROR;
            }
            stopAsMasterI2C(I2CDmaPointer);
        }
    }
}

void transmitStringI2C_DMA(I2C_DMA *I2CDmaPointer, char *dataToTransmit, uint32_t length, uint32_t address) {
    if (I2CDmaPointer->txData == NULL) return;
    length = length < I2CDmaPointer->txData->bufferSize ? length : I2CDmaPointer->txData->bufferSize;
    memcpy(I2CDmaPointer->txData->bufferPointer, dataToTransmit, length);
    I2CDmaPointer->status = startAsMasterI2C(I2CDmaPointer, address, I2C_WRITE_TO_SLAVE);
    if (I2CDmaPointer->status == I2C_OK) {
        LL_DMA_DisableStream(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream);
        LL_DMA_SetDataLength(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream, length);
        LL_DMA_EnableStream(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream);
    }
}

void transmitTxBufferI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address) {
    if (I2CDmaPointer->txData == NULL) return;
    I2CDmaPointer->status = startAsMasterI2C(I2CDmaPointer, address, I2C_WRITE_TO_SLAVE);
    if (I2CDmaPointer->status == I2C_OK) {
        LL_DMA_DisableStream(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream);
        LL_DMA_SetDataLength(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream, I2CDmaPointer->txData->bufferSize);
        LL_DMA_EnableStream(I2CDmaPointer->DMAx, I2CDmaPointer->txData->stream);
    }
}

void receiveRxBufferI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address) {
    if (I2CDmaPointer->rxData == NULL) return;
    I2CDmaPointer->status = startAsMasterI2C(I2CDmaPointer, address, I2C_READ_FROM_SLAVE);
    if (I2CDmaPointer->status == I2C_OK) {
        LL_DMA_DisableStream(I2CDmaPointer->DMAx, I2CDmaPointer->rxData->stream);
        LL_DMA_SetDataLength(I2CDmaPointer->DMAx, I2CDmaPointer->rxData->stream, I2CDmaPointer->rxData->bufferSize);
        LL_DMA_EnableStream(I2CDmaPointer->DMAx, I2CDmaPointer->rxData->stream);
    }
}

bool isTransferCompleteI2C_DMA(I2CData *I2CData) {
    if (I2CData->isTransferComplete) {
        I2CData->isTransferComplete = false;
        return true;
    }
    return false;
}

bool isDeviceReadyI2C_DMA(I2C_DMA *I2CDmaPointer, uint32_t address) {
    I2CStatus status = startAsMasterI2C(I2CDmaPointer, address, I2C_READ_FROM_SLAVE);
    stopAsMasterI2C(I2CDmaPointer);
    return status == I2C_OK;
}

void deleteI2C_DMA(I2C_DMA *I2CDmaPointer) {
    if (I2CDmaPointer != NULL) {
        if (I2CDmaPointer->rxData != NULL) {
            free(I2CDmaPointer->rxData->bufferPointer);
            free(I2CDmaPointer->rxData);
        }
        if (I2CDmaPointer->txData != NULL) {
            free(I2CDmaPointer->txData->bufferPointer);
            free(I2CDmaPointer->txData);
        }
        free(I2CDmaPointer);
    }
}

static I2C_DMA *cacheI2CInstance(I2C_DMA *I2CDmaInstance) {
    for (uint8_t i = 0; i < NUMBER_OF_I2C_INSTANCES; i++) {
        if (I2C_DMAInstanceArray[i] == NULL) {
            I2C_DMAInstanceArray[i] = I2CDmaInstance;
            return I2C_DMAInstanceArray[i];
        }
    }
    return NULL;
}

static I2CStatus startAsMasterI2C(I2C_DMA *I2CDmaInstance, uint32_t address, I2CDataDirection direction) {
    LL_I2C_DisableBitPOS(I2CDmaInstance->I2Cx);
    LL_I2C_AcknowledgeNextData(I2CDmaInstance->I2Cx, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2CDmaInstance->I2Cx);

    if (!loopWhileCheckFunctionStatusReset(I2CDmaInstance, LL_I2C_IsActiveFlag_SB)) {
        return I2C_START_ERROR;
    }
    return sendSlaveAddress(I2CDmaInstance, address, direction);
}

static void stopAsMasterI2C(I2C_DMA *I2CDmaInstance) {
    LL_I2C_GenerateStopCondition(I2CDmaInstance->I2Cx);
}

static I2CStatus sendSlaveAddress(I2C_DMA *I2CDmaInstance, uint32_t address, I2CDataDirection direction) {
    LL_I2C_ClearFlag_ADDR(I2CDmaInstance->I2Cx);

    if (I2CDmaInstance->addressingMode == I2C_ADDRESSING_MODE_7BIT) {  // 0: 7-bit slave address (10-bit address not acknowledged)
        write7BitSlaveAddress(I2CDmaInstance->I2Cx, address, direction);
    } else {
        write10BitHeaderSlaveAddress(I2CDmaInstance->I2Cx, address, direction);
        if (!loopWhileCheckFunctionStatusReset(I2CDmaInstance, LL_I2C_IsActiveFlag_ADD10)) {// Wait until ADD10 flag is set
            return I2C_ACK_ERROR;
        }
        write10BitSlaveAddress(I2CDmaInstance->I2Cx, address);   // Send slave address
    }

    if (!loopWhileCheckFunctionStatusReset(I2CDmaInstance, LL_I2C_IsActiveFlag_ADDR)) {// Wait until ADDR flag is set
        return I2C_ACK_ERROR;
    }
    LL_I2C_ClearFlag_ADDR(I2CDmaInstance->I2Cx);
    return I2C_OK;
}

static bool loopWhileCheckFunctionStatusReset(I2C_DMA *I2CDmaInstance, uint32_t (*checkFunctionPointer)(I2C_TypeDef *)) {
    uint32_t startTime = currentMilliseconds();
    while (checkFunctionPointer(I2CDmaInstance->I2Cx) == RESET) {
        if ((currentMilliseconds() - startTime) >= I2CDmaInstance->timeout) {
            return false;
        }
    }
    return true;
}
