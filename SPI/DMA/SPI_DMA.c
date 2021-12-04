#include "SPI_DMA.h"

static SPI_DMA *SPI_DMAInstanceArray[NUMBER_OF_SPI_INSTANCES] = {[0 ... NUMBER_OF_SPI_INSTANCES - 1] = NULL};

static SPIData *createSpiData(SPI_TypeDef *SPIx, uint32_t bufferSize);
static SPI_DMA *cacheSPIInstance(SPI_DMA *SPIDmaInstance);
static inline void chipSelectSetSPI(SPI_DMA *SPIDmaInstance);
static inline void chipSelectResetSPI(SPI_DMA *SPIDmaInstance);


SPI_DMA *initSPI_DMA(SPI_TypeDef *SPIx,
                     DMA_TypeDef *DMAx,
                     GPIO_TypeDef *chipSelectPort,
                     uint32_t chipSelectPin,
                     uint32_t rxStream,
                     uint32_t txStream,
                     uint32_t rxBufferSize,
                     uint32_t txBufferSize) {
    SPI_DMA *SPIDmaInstance = malloc(sizeof(SPI_DMA));
    if (SPIDmaInstance == NULL) return NULL;
    SPIDmaInstance->SPIx = SPIx;
    SPIDmaInstance->DMAx = DMAx;
    SPIDmaInstance->chipSelectPort = chipSelectPort;
    SPIDmaInstance->chipSelectPin = chipSelectPin;
    SPIDmaInstance->rxData = NULL;
    SPIDmaInstance->txData = NULL;

    if (rxBufferSize > 0) {
        SPIData *rxData = createSpiData(SPIx, rxBufferSize);
        if (rxData == NULL) {
            deleteSPI_DMA(SPIDmaInstance);
            return NULL;
        }
        enableDMAStream(DMAx, rxStream);
        LL_SPI_EnableDMAReq_RX(SPIx);

        uint32_t dataRegisterAddress = LL_SPI_DMA_GetRegAddr(SPIx);
        uint32_t rxDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, rxStream);
        uint32_t receiverBufferAddress = (LL_SPI_GetDataWidth(SPIx) == LL_SPI_DATAWIDTH_8BIT) ? (uint32_t) rxData->byteBufferPointer : (uint32_t) rxData->halfWordBufferPointer;
        LL_DMA_ConfigAddresses(DMAx, rxStream, dataRegisterAddress, receiverBufferAddress, rxDataTransferDirection);

        SPIDmaInstance->rxData = rxData;
        SPIDmaInstance->rxData->stream = rxStream;
        SPIDmaInstance->rxData->bufferSize = rxBufferSize;
        SPIDmaInstance->rxData->isTransferComplete = false;
    }

    if (txBufferSize > 0) {
        SPIData *txData = createSpiData(SPIx, txBufferSize);
        if (txData == NULL) {
            deleteSPI_DMA(SPIDmaInstance);
            return NULL;
        }
        enableDMAStream(DMAx, txStream);
        LL_SPI_EnableDMAReq_RX(SPIx);

        uint32_t dataRegisterAddress = LL_SPI_DMA_GetRegAddr(SPIx);
        uint32_t txDataTransferDirection = LL_DMA_GetDataTransferDirection(DMAx, txStream);
        uint32_t transmitterBufferAddress = (LL_SPI_GetDataWidth(SPIx) == LL_SPI_DATAWIDTH_8BIT) ? (uint32_t) txData->byteBufferPointer : (uint32_t) txData->halfWordBufferPointer;
        LL_DMA_ConfigAddresses(DMAx, txStream, transmitterBufferAddress, dataRegisterAddress, txDataTransferDirection);

        SPIDmaInstance->txData = txData;
        SPIDmaInstance->txData->stream = txStream;
        SPIDmaInstance->txData->bufferSize = txBufferSize;
        SPIDmaInstance->txData->isTransferComplete = false;
    }

    LL_SPI_EnableDMAReq_TX(SPIx);
    LL_SPI_EnableDMAReq_RX(SPIx);
    LL_SPI_Enable(SPIx);

    return cacheSPIInstance(SPIDmaInstance);
}

SPI_DMA *initSPI_DMA_RX(SPI_TypeDef *SPIx,
                        DMA_TypeDef *DMAx,
                        GPIO_TypeDef *chipSelectPort,
                        uint32_t chipSelectPin,
                        uint32_t rxStream,
                        uint32_t rxBufferSize) {
    return initSPI_DMA(SPIx, DMAx, chipSelectPort, chipSelectPin, rxStream, 0, rxBufferSize, 0);
}

SPI_DMA *initSPI_DMA_TX(SPI_TypeDef *SPIx,
                        DMA_TypeDef *DMAx,
                        GPIO_TypeDef *chipSelectPort,
                        uint32_t chipSelectPin,
                        uint32_t txStream,
                        uint32_t txBufferSize) {
    return initSPI_DMA(SPIx, DMAx, chipSelectPort, chipSelectPin, 0, txStream, 0, txBufferSize);
}

void transferCompleteCallbackSPI_DMA(DMA_TypeDef *DMAx, uint32_t stream) {
    for (uint8_t i = 0; i < NUMBER_OF_SPI_INSTANCES; i++) {
        SPI_DMA *SPIDmaPointer = SPI_DMAInstanceArray[i];
        if (SPIDmaPointer->DMAx == DMAx && SPIDmaPointer->rxData != NULL && SPIDmaPointer->rxData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(SPIDmaPointer->DMAx, stream)) {
                disableTransferCompleteInterruptDMA(SPIDmaPointer->DMAx, stream);
                SPIDmaPointer->rxData->isTransferComplete = true;
            } else if (isTransferErrorInterruptEnabledDMA(SPIDmaPointer->DMAx, stream)) {
                disableTransferErrorInterruptDMA(SPIDmaPointer->DMAx, stream);
            }
        } else if (SPIDmaPointer->DMAx == DMAx && SPIDmaPointer->txData != NULL && SPIDmaPointer->txData->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(SPIDmaPointer->DMAx, stream)) {
                disableTransferCompleteInterruptDMA(SPIDmaPointer->DMAx, stream);
                SPIDmaPointer->txData->isTransferComplete = true;
                chipSelectResetSPI(SPIDmaPointer);
            } else if (isTransferErrorInterruptEnabledDMA(SPIDmaPointer->DMAx, stream)) {
                disableTransferErrorInterruptDMA(SPIDmaPointer->DMAx, stream);
            }
        }
    }
}

void transmitStringSPI_DMA(SPI_DMA *SPIDmaPointer, char *dataToTransmit, uint32_t length) {
    if (SPIDmaPointer->txData == NULL || LL_SPI_GetDataWidth(SPIDmaPointer->SPIx) == LL_SPI_DATAWIDTH_16BIT) return;
    length = length < SPIDmaPointer->txData->bufferSize ? length : SPIDmaPointer->txData->bufferSize;
    memcpy(SPIDmaPointer->txData->byteBufferPointer, dataToTransmit, length);
    chipSelectSetSPI(SPIDmaPointer);
	SPIDmaPointer->txData->isTransferComplete = false;
    LL_DMA_DisableStream(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream);
    LL_DMA_SetDataLength(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream, length);
    LL_DMA_EnableStream(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream);
}

void transmitTxBufferSPI_DMA(SPI_DMA *SPIDmaPointer) {
    if (SPIDmaPointer->txData == NULL) return;
    chipSelectSetSPI(SPIDmaPointer);
	SPIDmaPointer->txData->isTransferComplete = false;
    LL_DMA_DisableStream(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream);
    LL_DMA_SetDataLength(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream, SPIDmaPointer->txData->bufferSize);
    LL_DMA_EnableStream(SPIDmaPointer->DMAx, SPIDmaPointer->txData->stream);
}

void receiveRxBufferSPI_DMA(SPI_DMA *SPIDmaPointer) {
    if (SPIDmaPointer->rxData == NULL) return;
	SPIDmaPointer->rxData->isTransferComplete = false;
    LL_DMA_DisableStream(SPIDmaPointer->DMAx, SPIDmaPointer->rxData->stream);
    LL_DMA_SetDataLength(SPIDmaPointer->DMAx, SPIDmaPointer->rxData->stream, SPIDmaPointer->rxData->bufferSize);
    LL_DMA_EnableStream(SPIDmaPointer->DMAx, SPIDmaPointer->rxData->stream);
}

bool isTransferCompleteSPI_DMA(SPIData *spiData) {
    if (spiData->isTransferComplete) {
        spiData->isTransferComplete = false;
        return true;
    }
    return false;
}

void deleteSPI_DMA(SPI_DMA *SPIDmaPointer) {
    if (SPIDmaPointer != NULL) {
        if (SPIDmaPointer->rxData != NULL) {
            free(SPIDmaPointer->rxData->byteBufferPointer);
            free(SPIDmaPointer->rxData);
        }
        if (SPIDmaPointer->rxData != NULL) {
            free(SPIDmaPointer->txData->byteBufferPointer);
            free(SPIDmaPointer->txData);
        }
        free(SPIDmaPointer);
    }
}

static SPIData *createSpiData(SPI_TypeDef *SPIx, uint32_t bufferSize) {
    if (bufferSize > 0) {
        SPIData *spiData = malloc(sizeof(SPIData));
        if (LL_SPI_GetDataWidth(SPIx) == LL_SPI_DATAWIDTH_8BIT) {
            uint8_t *dataBufferPointer = malloc(sizeof(uint8_t) * bufferSize);
            if (spiData == NULL || dataBufferPointer == NULL) {
                return NULL;
            }
            memset(dataBufferPointer, 0, bufferSize);
            spiData->byteBufferPointer = dataBufferPointer;

        } else {
            uint16_t *dataBufferPointer = malloc(sizeof(uint16_t) * bufferSize);
            if (spiData == NULL || dataBufferPointer == NULL) {
                return NULL;
            }
            memset(dataBufferPointer, 0, bufferSize);
            spiData->halfWordBufferPointer = dataBufferPointer;
        }
        return spiData;
    }
    return NULL;
}

static SPI_DMA *cacheSPIInstance(SPI_DMA *SPIDmaInstance) {
    for (uint8_t i = 0; i < NUMBER_OF_SPI_INSTANCES; i++) {
        if (SPI_DMAInstanceArray[i] == NULL) {
            SPI_DMAInstanceArray[i] = SPIDmaInstance;
            return SPI_DMAInstanceArray[i];
        }
    }
    return NULL;
}

static inline void chipSelectSetSPI(SPI_DMA *SPIDmaInstance) {
    if (LL_SPI_GetMode(SPIDmaInstance->SPIx) == LL_SPI_MODE_MASTER) {
        LL_GPIO_ResetOutputPin(SPIDmaInstance->chipSelectPort, SPIDmaInstance->chipSelectPin);
    }
}

static inline void chipSelectResetSPI(SPI_DMA *SPIDmaInstance) {
    if (LL_SPI_GetMode(SPIDmaInstance->SPIx) == LL_SPI_MODE_MASTER) {
        LL_GPIO_SetOutputPin(SPIDmaInstance->chipSelectPort, SPIDmaInstance->chipSelectPin);
    }
}
