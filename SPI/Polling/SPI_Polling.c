#include "SPI_Polling.h"

SPI_Polling initSPI(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin) {
    SPI_Polling spiInstance = {0};
    spiInstance.SPIx = SPIx;
    spiInstance.chipSelectPort = chipSelectPort;
    spiInstance.chipSelectPin = chipSelectPin;
    return spiInstance;
}

void chipSelectSet(SPI_Polling *spi) {
    LL_GPIO_ResetOutputPin(spi->chipSelectPort, spi->chipSelectPin);
}

void chipSelectReset(SPI_Polling *spi) {
    LL_GPIO_SetOutputPin(spi->chipSelectPort, spi->chipSelectPin);
}

void transmit8BitsSPI(SPI_Polling *spi, uint8_t byte) {
    transmitReceive8BitsSPI(spi, byte);
}

uint8_t receive8BitsSPI(SPI_Polling *spi) {
    while (!LL_SPI_IsActiveFlag_TXE(spi->SPIx));    // wait for TXE (Transmit buffer empty)
    LL_SPI_TransmitData8(spi->SPIx, 0xFF);  // the clock is controlled by master. Thus, the master should send a byte, 0xFF - a dummy byte
    while (!LL_SPI_IsActiveFlag_RXNE(spi->SPIx));   // data to the slave to start the clock
    return LL_SPI_ReceiveData8(spi->SPIx);
}

void transmit16BitsSPI(SPI_Polling *spi, uint16_t halfWord) {
    transmitReceive16BitsSPI(spi, halfWord);
}

uint16_t receive16BitsSPI(SPI_Polling *spi) {
    while (!LL_SPI_IsActiveFlag_TXE(spi->SPIx));
    LL_SPI_TransmitData16(spi->SPIx, 0xFFFF);
    while (!LL_SPI_IsActiveFlag_RXNE(spi->SPIx));
    return LL_SPI_ReceiveData16(spi->SPIx);
}

uint8_t transmitReceive8BitsSPI(SPI_Polling *spi, uint8_t byte) {
    if (!LL_SPI_IsEnabled(spi->SPIx)) LL_SPI_Enable(spi->SPIx);
    while (!LL_SPI_IsActiveFlag_TXE(spi->SPIx));
    LL_SPI_TransmitData8(spi->SPIx, byte);
    while (!LL_SPI_IsActiveFlag_RXNE(spi->SPIx));
    LL_SPI_Disable(spi->SPIx);
    return LL_SPI_ReceiveData8(spi->SPIx);
}

uint16_t transmitReceive16BitsSPI(SPI_Polling *spi, uint16_t halfWord) {
    if (!LL_SPI_IsEnabled(spi->SPIx)) LL_SPI_Enable(spi->SPIx);
    while (!LL_SPI_IsActiveFlag_TXE(spi->SPIx));
    LL_SPI_TransmitData16(spi->SPIx, halfWord);
    while (!LL_SPI_IsActiveFlag_RXNE(spi->SPIx));
    LL_SPI_Disable(spi->SPIx);
    return LL_SPI_ReceiveData16(spi->SPIx);
}

void transmit8BitDataSPI(SPI_Polling *spi, const uint8_t *txData, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        transmitReceive8BitsSPI(spi, txData[i]);
    }
    while (LL_SPI_IsActiveFlag_BSY(spi->SPIx)); // wait for BSY flag cleared
}

void transmit16BitDataSPI(SPI_Polling *spi, const uint16_t *txData, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        transmit16BitsSPI(spi, txData[i]);
    }
    while (LL_SPI_IsActiveFlag_BSY(spi->SPIx));
}

void receive8bitDataSPI(SPI_Polling *spi, uint8_t *rxData, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        rxData[i] = receive8BitsSPI(spi);
    }
    while (LL_SPI_IsActiveFlag_BSY(spi->SPIx));
}

void receive16bitDataSPI(SPI_Polling *spi, uint16_t *rxData, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        rxData[i] = receive16BitsSPI(spi);
    }
    while (LL_SPI_IsActiveFlag_BSY(spi->SPIx));
}