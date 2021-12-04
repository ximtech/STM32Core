#include "SPI_IT.h"

#define NUMBER_OF_SPI_INSTANCES 3

#define FIRST_SPI_INSTANCE_INDEX  0
#define SECOND_SPI_INSTANCE_INDEX 1
#define THIRD_SPI_INSTANCE_INDEX  2

static SPI_IT SPIInstanceArray[NUMBER_OF_SPI_INSTANCES];

static SPI_IT *cacheSPIInstance(SPI_IT SPIInstance);
static void interruptCallbackHandler(SPI_IT *SPIPointer);
static void rxInterruptCallbackSPI(SPI_IT *SPIPointer);
static void txInterruptCallbackSPI(SPI_IT *SPIPointer);


SPI_IT *initSPI_IT(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxBufferSize, uint32_t txBufferSize) {
    SPI_IT SPIInstance = {0};
    SPIInstance.SPIx = SPIx;
    SPIInstance.chipSelectPort = chipSelectPort;
    SPIInstance.chipSelectPin = chipSelectPin;
    SPIInstance.RxBuffer = getRingBufferInstance(rxBufferSize);
    SPIInstance.TxBuffer = getRingBufferInstance(txBufferSize);

    LL_SPI_EnableIT_RXNE(SPIx);
    LL_SPI_EnableIT_ERR(SPIx);
    LL_SPI_Enable(SPIx);
    return cacheSPIInstance(SPIInstance);
}

void interruptCallbackSPI1() {
    SPI_IT *SPIInstancePointer = &SPIInstanceArray[FIRST_SPI_INSTANCE_INDEX];
    interruptCallbackHandler(SPIInstancePointer);
}

void interruptCallbackSPI2() {
    SPI_IT *SPIInstancePointer = &SPIInstanceArray[SECOND_SPI_INSTANCE_INDEX];
    interruptCallbackHandler(SPIInstancePointer);
}

void interruptCallbackSPI3() {
    SPI_IT *SPIInstancePointer = &SPIInstanceArray[THIRD_SPI_INSTANCE_INDEX];
    interruptCallbackHandler(SPIInstancePointer);
}

void chipSelectSetSPI_IT(SPI_IT *SPIPointer) {
    LL_GPIO_ResetOutputPin(SPIPointer->chipSelectPort, SPIPointer->chipSelectPin);
}

void chipSelectResetSPI_IT(SPI_IT *SPIPointer) {
    LL_GPIO_SetOutputPin(SPIPointer->chipSelectPort, SPIPointer->chipSelectPin);
}

void transmitDataSPI_IT(SPI_IT *SPIPointer, RingBufferDataType data) {
    while (isRingBufferFull(SPIPointer->TxBuffer));
    chipSelectSetSPI_IT(SPIPointer);
    ringBufferAdd(SPIPointer->TxBuffer, data);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

RingBufferDataType receiveDataSPI_IT(SPI_IT *SPIPointer) {
    ringBufferGet(SPIPointer->RxBuffer);
    return ringBufferGet(SPIPointer->RxBuffer);;
}

void transmitMultipleDataSPI_IT(SPI_IT *SPIPointer, const RingBufferDataType *txData, uint16_t length) {
    while(!LL_SPI_IsActiveFlag_TXE(SPIPointer->SPIx));  // wait if line is busy
    for (uint16_t i = 0; i < length; i++) {
        if (isRingBufferNotFull(SPIPointer->TxBuffer)) {
            ringBufferAdd(SPIPointer->TxBuffer, txData[i]);
        } else {
            LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
            while (isRingBufferFull(SPIPointer->TxBuffer));
            ringBufferAdd(SPIPointer->TxBuffer, txData[i]);
        }
    }
    chipSelectSetSPI_IT(SPIPointer);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

void receiveMultipleDataSPI_IT(SPI_IT *SPIPointer, RingBufferDataType *rxData, uint16_t length) {
    chipSelectSetSPI_IT(SPIPointer);
    while (LL_SPI_IsActiveFlag_BSY(SPIPointer->SPIx));
    uint8_t byte = 0;
    for (uint16_t i = 0; isRingBufferNotEmpty(SPIPointer->RxBuffer) && (i < length); i++) {
        rxData[i] = ringBufferGet(SPIPointer->RxBuffer);
    }
    chipSelectResetSPI_IT(SPIPointer);
}

bool isRxBufferEmptySPI(SPI_IT *SPIPointer) {
    return isRingBufferEmpty(SPIPointer->RxBuffer);
}

bool isRxBufferNotEmptySPI(SPI_IT *SPIPointer) {
    return isRingBufferNotEmpty(SPIPointer->RxBuffer);
}

bool isTxBufferEmptySPI(SPI_IT *SPIPointer) {
    return isRingBufferEmpty(SPIPointer->TxBuffer);
}

bool isTxBufferNotEmptySPI(SPI_IT *SPIPointer) {
    return isRingBufferNotEmpty(SPIPointer->TxBuffer);
}

void resetRxSPI_IT(SPI_IT *SPIPointer) {
    ringBufferReset(SPIPointer->RxBuffer);
}

void resetTxSPI_IT(SPI_IT *SPIPointer) {
    ringBufferReset(SPIPointer->TxBuffer);
}

void deleteSPI_IT(SPI_IT *SPIPointer) {
    if (SPIPointer != NULL) {
        ringBufferDelete(SPIPointer->RxBuffer);
        ringBufferDelete(SPIPointer->TxBuffer);
        free(SPIPointer);
    }
}

static SPI_IT *cacheSPIInstance(SPI_IT SPIInstance) {
    if (SPIInstance.SPIx == SPI1) {
        SPIInstanceArray[FIRST_SPI_INSTANCE_INDEX] = SPIInstance;
        return &SPIInstanceArray[FIRST_SPI_INSTANCE_INDEX];
    } else if (SPIInstance.SPIx == SPI2) {
        SPIInstanceArray[SECOND_SPI_INSTANCE_INDEX] = SPIInstance;
        return &SPIInstanceArray[SECOND_SPI_INSTANCE_INDEX];
    } else if (SPIInstance.SPIx == SPI3) {
        SPIInstanceArray[THIRD_SPI_INSTANCE_INDEX] = SPIInstance;
        return &SPIInstanceArray[THIRD_SPI_INSTANCE_INDEX];
    }
    return NULL;
}

static void interruptCallbackHandler(SPI_IT *SPIPointer) {
    if (LL_SPI_IsActiveFlag_RXNE(SPIPointer->SPIx)) {
        rxInterruptCallbackSPI(SPIPointer);
    } else if (LL_SPI_IsActiveFlag_TXE(SPIPointer->SPIx)) {
        txInterruptCallbackSPI(SPIPointer);
    } else if (LL_SPI_IsActiveFlag_OVR(SPIPointer->SPIx)) {
        __NOP();    // do nothing
    }
}

static void rxInterruptCallbackSPI(SPI_IT *SPIPointer) {
    if (LL_SPI_GetDataWidth(SPIPointer->SPIx) == LL_SPI_DATAWIDTH_8BIT) {
        uint8_t byte = LL_SPI_ReceiveData8(SPIPointer->SPIx);
        ringBufferAdd(SPIPointer->RxBuffer, byte);
    } else {
        uint16_t halfWord = LL_SPI_ReceiveData16(SPIPointer->SPIx);
        ringBufferAdd(SPIPointer->RxBuffer, halfWord);
    }
}

static void txInterruptCallbackSPI(SPI_IT *SPIPointer) {
    if (isRingBufferNotEmpty(SPIPointer->TxBuffer)) { // if tx buffer not empty get byte
        if (LL_SPI_GetDataWidth(SPIPointer->SPIx) == LL_SPI_DATAWIDTH_8BIT) {
            LL_SPI_TransmitData8(SPIPointer->SPIx, ringBufferGet(SPIPointer->TxBuffer));
        } else {
            LL_SPI_TransmitData16(SPIPointer->SPIx, ringBufferGet(SPIPointer->TxBuffer));
        }
    }  else {
        LL_SPI_DisableIT_TXE(SPIPointer->SPIx);
        chipSelectResetSPI_IT(SPIPointer);// tx byteBuffer empty, disable interrupt
    }
}
