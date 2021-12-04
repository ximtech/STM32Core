#include "SPI_IT.h"

#define NUMBER_OF_SPI_INSTANCES 3

#define FIRST_SPI_INSTANCE_INDEX  0
#define SECOND_SPI_INSTANCE_INDEX 1
#define THIRD_SPI_INSTANCE_INDEX  2

static SPI_IT SPIInstanceArray[NUMBER_OF_SPI_INSTANCES];

static SPI_IT *cacheSPIInstance(SPI_IT SPIInstance);
static void interruptCallbackHandler(BufferedSPI *SPIPointer);
static void rxInterruptCallbackSPI(BufferedSPI *SPIPointer);
static void txInterruptCallbackSPI(BufferedSPI *SPIPointer);
static void completeDataTransmission(BufferedSPI *SPIPointer);


SPI_IT *initBufferedSPI(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin, uint32_t rxBufferSize, uint32_t txBufferSize) {
    SPI_IT SPIInstance = {0};
    SPIInstance.SPIx = SPIx;
    SPIInstance.chipSelectPort = chipSelectPort;
    SPIInstance.chipSelectPin = chipSelectPin;

    if (LL_SPI_GetDataWidth(SPIx) == LL_SPI_DATAWIDTH_8BIT) {
        SPIInstance.RxBuffer = getRingBufferInstance(BYTE_BUFFER, rxBufferSize);
        SPIInstance.TxBuffer = getRingBufferInstance(BYTE_BUFFER, txBufferSize);
    } else {
        SPIInstance.RxBuffer = getRingBufferInstance(HALF_WORD_BUFFER, rxBufferSize);
        SPIInstance.TxBuffer = getRingBufferInstance(HALF_WORD_BUFFER, txBufferSize);
    }

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

void chipSelectSetBufferedSPI(SPI_IT *SPIPointer) {
    LL_GPIO_ResetOutputPin(SPIPointer->chipSelectPort, SPIPointer->chipSelectPin);
}

void chipSelectResetBufferedSPI(SPI_IT *SPIPointer) {
    LL_GPIO_SetOutputPin(SPIPointer->chipSelectPort, SPIPointer->chipSelectPin);
}

void transmit8BitsBufferedSPI(SPI_IT *SPIPointer, uint8_t byte) {
    while (isFull(SPIPointer->TxBuffer));
    chipSelectSetBufferedSPI(SPIPointer);
    addByte(SPIPointer->TxBuffer, byte);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

void transmit16BitsBufferedSPI(SPI_IT *SPIPointer, uint16_t halfWord) {
    while (isFull(SPIPointer->TxBuffer));
    chipSelectSetBufferedSPI(SPIPointer);
    add16Bits(SPIPointer->TxBuffer, halfWord);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

uint8_t receive8BitsBufferedSPI(SPI_IT *SPIPointer) {
    uint8_t byte = 0;
    getByte(SPIPointer->RxBuffer, &byte);
    return byte;
}

uint16_t receive16BitsBufferedSPI(SPI_IT *SPIPointer) {
    uint16_t halfWord = 0;
    get16Bits(SPIPointer->RxBuffer, &halfWord);
    return halfWord;
}

void transmit8BitDataBufferedSPI(SPI_IT *SPIPointer, const uint8_t *txData, uint16_t length) {
    while(!LL_SPI_IsActiveFlag_TXE(SPIPointer->SPIx));  // wait if line is busy
    for (uint16_t i = 0; i < length; i++) {
        if (isNotFull(SPIPointer->TxBuffer)) {
            addByte(SPIPointer->TxBuffer, txData[i]);
        } else {
            LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
            while (isFull(SPIPointer->TxBuffer));
            addByte(SPIPointer->TxBuffer, txData[i]);
        }
    }
    chipSelectSetBufferedSPI(SPIPointer);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

void transmit16BitDataBufferedSPI(SPI_IT *SPIPointer, const uint16_t *txData, uint16_t length) {
    while(!LL_SPI_IsActiveFlag_TXE(SPIPointer->SPIx));  // wait if line is busy
    for (uint16_t i = 0; i < length; i++) {
        if (isNotFull(SPIPointer->TxBuffer)) {
            add16Bits(SPIPointer->TxBuffer, txData[i]);
        } else {
            LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
            while (isFull(SPIPointer->TxBuffer));
            add16Bits(SPIPointer->TxBuffer, txData[i]);
        }
    }
    chipSelectSetBufferedSPI(SPIPointer);
    LL_SPI_EnableIT_TXE(SPIPointer->SPIx);
}

void receive8bitDataBufferedSPI(SPI_IT *SPIPointer, uint8_t *rxData, uint16_t length) {
    chipSelectSetBufferedSPI(SPIPointer);
    while (LL_SPI_IsActiveFlag_BSY(SPIPointer->SPIx));
    uint8_t byte = 0;
    for (uint16_t i = 0; getByte(SPIPointer->RxBuffer, &byte) && (i < length); i++) {
        rxData[i] = byte;
    }
    chipSelectResetBufferedSPI(SPIPointer);
}

void receive16bitDataBufferedSPI(SPI_IT *SPIPointer, uint16_t *rxData, uint16_t length) {
    chipSelectSetBufferedSPI(SPIPointer);
    while (LL_SPI_IsActiveFlag_BSY(SPIPointer->SPIx));
    uint16_t halfWord = 0;
    for (uint16_t i = 0; get16Bits(SPIPointer->RxBuffer, &halfWord) && (i < length); i++) {
        rxData[i] = halfWord;
    }
    chipSelectResetBufferedSPI(SPIPointer);
}

bool isRxBufferEmptySPI(SPI_IT *SPIPointer) {
    return isEmpty(SPIPointer->RxBuffer);
}

bool isRxBufferNotEmptySPI(SPI_IT *SPIPointer) {
    return isNotEmpty(SPIPointer->RxBuffer);
}

bool isTxBufferEmptySPI(SPI_IT *SPIPointer) {
    return isEmpty(SPIPointer->TxBuffer);
}

bool isTxBufferNotEmptySPI(SPI_IT *SPIPointer) {
    return isNotEmpty(SPIPointer->TxBuffer);
}

void resetRxBufferedSPI(SPI_IT *SPIPointer) {
    reset(SPIPointer->RxBuffer);
}

void resetTxBufferedSPI(SPI_IT *SPIPointer) {
    reset(SPIPointer->TxBuffer);
}

void deleteSPI(SPI_IT *SPIPointer) {
    delete(SPIPointer->RxBuffer);
    delete(SPIPointer->TxBuffer);
    free(SPIPointer);
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
        addByte(SPIPointer->RxBuffer, byte);
    } else {
        uint16_t halfWord = LL_SPI_ReceiveData16(SPIPointer->SPIx);
        add16Bits(SPIPointer->RxBuffer, halfWord);
    }
}

static void txInterruptCallbackSPI(SPI_IT *SPIPointer) {
    if (LL_SPI_GetDataWidth(SPIPointer->SPIx) == LL_SPI_DATAWIDTH_8BIT) {
        uint8_t byte = 0;
        if (getByte(SPIPointer->TxBuffer, &byte)) { // if tx buffer not empty get byte
            LL_SPI_TransmitData8(SPIPointer->SPIx, byte);
        } else {
            completeDataTransmission(SPIPointer);// tx byteBuffer empty, disable interrupt
        }
    } else {
        uint16_t halfWord = 0;
        if (get16Bits(SPIPointer->TxBuffer, &halfWord)) {
            LL_SPI_TransmitData16(SPIPointer->SPIx, halfWord);
        } else {
            completeDataTransmission(SPIPointer);
        }
    }
}

static void completeDataTransmission(SPI_IT *SPIPointer) {
    LL_SPI_DisableIT_TXE(SPIPointer->SPIx);
    chipSelectResetBufferedSPI(SPIPointer);
}
