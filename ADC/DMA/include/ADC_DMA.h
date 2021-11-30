#pragma once

#include <string.h>
#include "ADCBase.h"
#include "DMAUtils.h"
#include "Vector.h"

typedef struct ADC_DMA {
    ADC_TypeDef *ADCx;
    DMA_TypeDef *DMAx;
    ADCStatus status;
    uint32_t stream;
    uint32_t bufferSize;
    uint32_t *bufferPointer;
    bool isTransferComplete;
} ADC_DMA;

ADC_DMA *initADC_DMA(ADC_TypeDef *ADCx, DMA_TypeDef *DMAx, uint32_t bufferSize, uint32_t stream);

void transferCompleteCallbackADC_DMA(DMA_TypeDef *DMAx, uint32_t stream);// Call this subroutine as callback at stm32f4xx_it.c, see attached example

void startADC_DMA(ADC_DMA *ADCPointer);
void stopADC_DMA(ADC_DMA *ADCPointer);
void readConversionDataADC_DMA(ADC_DMA *ADCPointer);

bool isTransferCompleteADC_DMA(ADC_DMA *ADCPointer);    // auto sets flag to false

void deleteADC_DMA(ADC_DMA *AdcDmaPointer);