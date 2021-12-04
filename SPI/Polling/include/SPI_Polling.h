#pragma once

#include <stdbool.h>
#include "main.h"

typedef struct SPI_Polling {
    SPI_TypeDef *SPIx;
    GPIO_TypeDef *chipSelectPort;
    uint32_t chipSelectPin;
} SPI_Polling;

SPI_Polling initSPI(SPI_TypeDef *SPIx, GPIO_TypeDef *chipSelectPort, uint32_t chipSelectPin);
void chipSelectSet(SPI_Polling *spi);
void chipSelectReset(SPI_Polling *spi);

void transmit8BitsSPI(SPI_Polling *spi, uint8_t byte);
void transmit16BitsSPI(SPI_Polling *spi, uint16_t halfWord);

uint8_t receive8BitsSPI(SPI_Polling *spi);
uint16_t receive16BitsSPI(SPI_Polling *spi);

uint8_t transmitReceive8BitsSPI(SPI_Polling *spi, uint8_t byte);
uint16_t transmitReceive16BitsSPI(SPI_Polling *spi, uint16_t halfWord);

void transmit8BitDataSPI(SPI_Polling *spi, const uint8_t *txData, uint16_t length);
void transmit16BitDataSPI(SPI_Polling *spi, const uint16_t *txData, uint16_t length);

void receive8bitDataSPI(SPI_Polling *spi, uint8_t *rxData, uint16_t length);
void receive16bitDataSPI(SPI_Polling *spi, uint16_t *rxData, uint16_t length);