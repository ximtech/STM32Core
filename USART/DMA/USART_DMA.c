#include "USART_DMA.h"

static USART_DMA *USART_DMAInstanceArray[NUMBER_OF_USART_DMA_INSTANCES] = {[0 ... NUMBER_OF_USART_DMA_INSTANCES - 1] = NULL};

static USART_DMA *cacheUSARTInstance(USART_DMA *USARTDmaInstance);
static void interruptCallbackHandler(USART_DMA *USARTDmaPointer);
static void clearInterruptFlag(USART_DMA *USARTDmaPointer);


USART_DMA *initUSART_DMA(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t txStream, uint32_t rxBufferSize, uint32_t txBufferSize) {
    USART_DMA *USARTDmaInstance = malloc(sizeof(USART_DMA));
    if (USARTDmaInstance == NULL) return NULL;
    USARTDmaInstance->USARTx = USARTx;
    USARTDmaInstance->DMAx = DMAx;
	USARTDmaInstance->rxData = NULL;
	USARTDmaInstance->txData = NULL;

    if (rxBufferSize > 0) {
        USARTData *rxData = malloc(sizeof(USARTData));
        char *rxBufferPointer = malloc(sizeof(char) * rxBufferSize);
        if (rxData == NULL || rxBufferPointer == NULL) {
            deleteUSART_DMA(USARTDmaInstance);
            return NULL;
        }
        USARTDmaInstance->rxData = rxData;
        memset(rxBufferPointer, 0, rxBufferSize);

        enableDMAStream(DMAx, rxStream);
        LL_USART_EnableDMAReq_RX(USARTx);

        uint32_t dataRegisterAddress = LL_USART_DMA_GetRegAddr(USARTx);
        uint32_t rxDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, rxStream);
        LL_DMA_ConfigAddresses(DMAx, rxStream, dataRegisterAddress, (uint32_t) rxBufferPointer, rxDataTransferDirection);

        USARTDmaInstance->rxData->stream = rxStream;
        USARTDmaInstance->rxData->bufferSize = rxBufferSize;
        USARTDmaInstance->rxData->bufferPointer = rxBufferPointer;
        USARTDmaInstance->rxData->isTransferComplete = false;
    }

    if (txBufferSize > 0) {
        USARTData *txData = malloc(sizeof(USARTData));
        char *txBufferPointer = malloc(sizeof(char) * txBufferSize);
        if (txData == NULL || txBufferPointer == NULL) {
            deleteUSART_DMA(USARTDmaInstance);
            return NULL;
        }
        USARTDmaInstance->txData = txData;
        memset(txBufferPointer, 0, txBufferSize);

        enableDMAStream(DMAx, txStream);
        LL_USART_EnableDMAReq_TX(USARTx);

        uint32_t dataRegisterAddress = LL_USART_DMA_GetRegAddr(USARTx);
        uint32_t txDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, txStream);
        LL_DMA_ConfigAddresses(DMAx, txStream, (uint32_t) txBufferPointer, dataRegisterAddress, txDataTransferDirection);

        USARTDmaInstance->txData->stream = txStream;
        USARTDmaInstance->txData->bufferSize = txBufferSize;
        USARTDmaInstance->txData->bufferPointer = txBufferPointer;
        USARTDmaInstance->txData->isTransferComplete = false;
    }

    LL_USART_EnableIT_RXNE(USARTx);
    LL_USART_EnableIT_ERROR(USARTx);
    LL_USART_EnableIT_IDLE(USARTx);

    return cacheUSARTInstance(USARTDmaInstance);
}

USART_DMA *initUSART_DMA_RX(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t rxStream, uint32_t rxBufferSize) {
    return initUSART_DMA(USARTx, DMAx, rxStream, 0, rxBufferSize, 0);
}

USART_DMA *initUSART_DMA_TX(USART_TypeDef *USARTx, DMA_TypeDef *DMAx, uint32_t txStream, uint32_t txBufferSize) {
    return initUSART_DMA(USARTx, DMAx, 0, txStream, 0, txBufferSize);
}

void transferCompleteCallbackUSART_DMA(DMA_TypeDef *DMAx, uint32_t stream) {
    for (uint8_t i = 0; i < NUMBER_OF_USART_DMA_INSTANCES; i++) {
        USART_DMA *USARTDmaPointer = USART_DMAInstanceArray[i];
        if (USARTDmaPointer->DMAx == DMAx && USARTDmaPointer->rxData != NULL && USARTDmaPointer->rxData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(USARTDmaPointer->DMAx, stream)) {
                disableTransferCompleteInterruptDMA(DMAx, stream);
                USARTDmaPointer->rxData->isTransferComplete = true;
            } else if (isTransferErrorInterruptEnabledDMA(USARTDmaPointer->DMAx, stream)) {
                disableTransferErrorInterruptDMA(USARTDmaPointer->DMAx, stream);
            }
        } else if (USARTDmaPointer->DMAx == DMAx && USARTDmaPointer->txData != NULL && USARTDmaPointer->txData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(USARTDmaPointer->DMAx, stream)) {
                disableTransferCompleteInterruptDMA(DMAx, stream);
                USARTDmaPointer->txData->isTransferComplete = true;
            } else if (isTransferErrorInterruptEnabledDMA(USARTDmaPointer->DMAx, stream)) {
                disableTransferErrorInterruptDMA(USARTDmaPointer->DMAx, stream);
            }
        }
    }
}

void interruptCallbackUSART(USART_TypeDef *USARTx) {
    for (uint8_t i = 0; i < NUMBER_OF_USART_DMA_INSTANCES; i++) {
        if (USART_DMAInstanceArray[i] != NULL && USART_DMAInstanceArray[i]->USARTx == USARTx) {
            interruptCallbackHandler(USART_DMAInstanceArray[i]);
            break;
        }
    }
}

void transmitUSART_DMA(USART_DMA *USARTDmaPointer, char *dataToTransmit, uint32_t length) {
    if (USARTDmaPointer->txData == NULL) return;
    length = length < USARTDmaPointer->txData->bufferSize ? length : USARTDmaPointer->txData->bufferSize;
    memcpy(USARTDmaPointer->txData->bufferPointer, dataToTransmit, length);
    LL_DMA_DisableStream(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream);
    LL_DMA_SetDataLength(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream, length);
    LL_DMA_EnableStream(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream);
    while (!USARTDmaPointer->txData->isTransferComplete);
    memset(USARTDmaPointer->txData->bufferPointer, 0, USARTDmaPointer->txData->bufferSize);
    USARTDmaPointer->txData->isTransferComplete = false;
}

void receiveUSART_DMA(USART_DMA *USARTDmaPointer, char *dataToReceive, uint32_t length) {
    if (USARTDmaPointer->rxData == NULL) return;
    LL_DMA_DisableStream(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream);
    LL_DMA_SetDataLength(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream, USARTDmaPointer->rxData->bufferSize);
    LL_DMA_EnableStream(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream);
    while (!USARTDmaPointer->rxData->isTransferComplete);
    memcpy(dataToReceive, USARTDmaPointer->rxData->bufferPointer, length);
    memset(USARTDmaPointer->rxData->bufferPointer, 0, USARTDmaPointer->rxData->bufferSize);
    USARTDmaPointer->rxData->isTransferComplete = false;
}

void transmitTxBufferUSART_DMA(USART_DMA *USARTDmaPointer) {
    if (USARTDmaPointer->txData == NULL) return;
    LL_DMA_DisableStream(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream);
    LL_DMA_SetDataLength(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream, USARTDmaPointer->txData->bufferSize);
    LL_DMA_EnableStream(USARTDmaPointer->DMAx, USARTDmaPointer->txData->stream);
}

void receiveRxBufferUSART_DMA(USART_DMA *USARTDmaPointer) {
    if (USARTDmaPointer->rxData == NULL) return;
    LL_DMA_DisableStream(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream);
    LL_DMA_SetDataLength(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream, USARTDmaPointer->rxData->bufferSize);
    LL_DMA_EnableStream(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream);
}

bool isTransferCompleteUSART_DMA(USARTData *usartData) {
    if (usartData->isTransferComplete) {
        usartData->isTransferComplete = false;
        return true;
    }
    return false;
}

void deleteUSART_DMA(USART_DMA *USARTDmaPointer) {
    if (USARTDmaPointer != NULL) {
        if (USARTDmaPointer->rxData != NULL) {
            free(USARTDmaPointer->rxData->bufferPointer);
            free(USARTDmaPointer->rxData);
        }
        if (USARTDmaPointer->txData != NULL) {
            free(USARTDmaPointer->txData->bufferPointer);
            free(USARTDmaPointer->txData);
        }
        free(USARTDmaPointer);
    }
}

static USART_DMA *cacheUSARTInstance(USART_DMA *USARTDmaInstance) {
    for (uint8_t i = 0; i < NUMBER_OF_USART_DMA_INSTANCES; i++) {
        if (USART_DMAInstanceArray[i] == NULL) {
            USART_DMAInstanceArray[i] = USARTDmaInstance;
            return USART_DMAInstanceArray[i];
        }
    }
    return NULL;
}

static void interruptCallbackHandler(USART_DMA *USARTDmaPointer) {
    if (LL_USART_IsActiveFlag_RXNE(USARTDmaPointer->USARTx) && LL_USART_IsEnabledIT_RXNE(USARTDmaPointer->USARTx)) {
        LL_USART_ClearFlag_RXNE(USARTDmaPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_IDLE(USARTDmaPointer->USARTx)) {
        LL_USART_ClearFlag_IDLE(USARTDmaPointer->USARTx);
        if (USARTDmaPointer->rxData != NULL) {
            LL_DMA_DisableStream(USARTDmaPointer->DMAx, USARTDmaPointer->rxData->stream);
            USARTDmaPointer->rxData->isTransferComplete = true;
        }
    } else {
        clearInterruptFlag(USARTDmaPointer);
    }
}

static void clearInterruptFlag(USART_DMA *USARTDmaPointer) {
    if (LL_USART_IsActiveFlag_ORE(USARTDmaPointer->USARTx)) {
        LL_USART_ClearFlag_ORE(USARTDmaPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_FE(USARTDmaPointer->USARTx)) {
        LL_USART_ClearFlag_FE(USARTDmaPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_NE(USARTDmaPointer->USARTx)) {
        LL_USART_ClearFlag_NE(USARTDmaPointer->USARTx);
    }
}