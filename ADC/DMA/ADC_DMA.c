#include "ADC_DMA.h"

#define INITIAL_NUMBER_OF_ADC_DMA_INSTANCES 1
#define ADC_STABILIZATION_TIME_US 3

static Vector ADCInstanceCache = NULL;

static void resolveExternalTriggering(ADC_DMA *ADCPointer);


ADC_DMA *initADC_DMA(ADC_TypeDef *ADCx, DMA_TypeDef *DMAx, uint32_t bufferSize, uint32_t stream) {
    if (bufferSize < 1) return NULL;
    ADC_DMA *AdcDmaPointer = malloc(sizeof(ADC_DMA));
    if (AdcDmaPointer == NULL) return NULL;

    uint32_t *dataBufferPointer = malloc(sizeof(uint32_t) * bufferSize);
    if (dataBufferPointer == NULL) {
        deleteADC_DMA(AdcDmaPointer);
        return NULL;
    }
    memset(dataBufferPointer, 0, bufferSize);
    enableDMAStream(DMAx, stream);

    AdcDmaPointer->ADCx = ADCx;
    AdcDmaPointer->DMAx = DMAx;
    AdcDmaPointer->status = ADC_OK;
    AdcDmaPointer->stream = stream;
    AdcDmaPointer->bufferSize = bufferSize;
    AdcDmaPointer->bufferPointer = dataBufferPointer;
    AdcDmaPointer->isTransferComplete = false;

    uint32_t dmaConfiguration = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dmaConfiguration |= LL_DMA_MODE_CIRCULAR;
    dmaConfiguration |= LL_DMA_PERIPH_NOINCREMENT;
    dmaConfiguration |= LL_DMA_MEMORY_INCREMENT;
    dmaConfiguration |= LL_DMA_PDATAALIGN_HALFWORD;
    dmaConfiguration |= LL_DMA_MDATAALIGN_HALFWORD;
    dmaConfiguration |= LL_DMA_PRIORITY_HIGH;
    LL_DMA_ConfigTransfer(DMAx, stream, dmaConfiguration);

    uint32_t sourceAddress = LL_ADC_DMA_GetRegAddr(ADCx, LL_ADC_DMA_REG_REGULAR_DATA);
    LL_DMA_ConfigAddresses(DMAx, stream, sourceAddress, (uint32_t) AdcDmaPointer->bufferPointer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetDataLength(DMAx, stream, bufferSize);

    initSingletonVector(&ADCInstanceCache, INITIAL_NUMBER_OF_ADC_DMA_INSTANCES);
    vectorAdd(ADCInstanceCache, AdcDmaPointer);
	LL_ADC_EnableIT_OVR(ADCx);
    return AdcDmaPointer;
}

void transferCompleteCallbackADC_DMA(DMA_TypeDef *DMAx, uint32_t stream) {
    for (uint32_t i = 0; i < getVectorSize(ADCInstanceCache); i++) {
        ADC_DMA *ADCPointer = vectorGet(ADCInstanceCache, i);
        if (ADCPointer != NULL && ADCPointer->DMAx == DMAx && ADCPointer->stream == stream) {
            if (isTransferCompleteInterruptEnabledDMA(ADCPointer->DMAx, stream)) {
                disableTransferCompleteInterruptDMA(ADCPointer->DMAx, stream);
                ADCPointer->status = ADC_OK;
                ADCPointer->isTransferComplete = true;
            } else if (isTransferErrorInterruptEnabledDMA(ADCPointer->DMAx, stream)) {
                disableTransferErrorInterruptDMA(ADCPointer->DMAx, stream);
                ADCPointer->status = ADC_DMA_ERROR;
            }
        }
    }
}

void startADC_DMA(ADC_DMA *ADCPointer) {
    LL_ADC_Enable(ADCPointer->ADCx);
    delay_us(ADC_STABILIZATION_TIME_US);
    LL_DMA_EnableStream(ADCPointer->DMAx, ADCPointer->stream);

    if (READ_BIT(ADCPointer->ADCx->CR2, ADC_CR2_ADON) != ADC_CR2_ADON) {    // Start conversion if ADC is effectively enabled
        ADCPointer->status = ADC_INTERNAL_ERROR;
        stopADC_DMA(ADCPointer);
        return;
    }

    if (LL_ADC_REG_IsTriggerSourceSWStart(ADCPointer->ADCx)) {  // software manual triggering
        LL_ADC_REG_StartConversionSWStart(ADCPointer->ADCx);    // initial measure, start continuous conversion mode if enabled
        delay_ms(100);
    } else {
        resolveExternalTriggering(ADCPointer);
    }
    ADCPointer->status = ADC_OK;
}

void stopADC_DMA(ADC_DMA *ADCPointer) {
    LL_ADC_Disable(ADCPointer->ADCx);
    LL_DMA_DisableStream(ADCPointer->DMAx, ADCPointer->stream);
}

void readConversionDataADC_DMA(ADC_DMA *ADCPointer) {
    if (LL_ADC_REG_GetContinuousMode(ADCPointer->ADCx) == LL_ADC_REG_CONV_SINGLE) { // call if single measure mode is enabled
        LL_ADC_REG_StartConversionSWStart(ADCPointer->ADCx);
    }
}

bool isTransferCompleteADC_DMA(ADC_DMA *ADCPointer) {
    if (ADCPointer->isTransferComplete) {
        ADCPointer->isTransferComplete = false;
        return true;
    }
    return false;
}

void deleteADC_DMA(ADC_DMA *ADCPointer) {
    if (ADCPointer != NULL) {
        free(ADCPointer->bufferPointer);
        free(ADCPointer);
    }
	vectorDelete(ADCInstanceCache);
    ADCInstanceCache = NULL;
}

static void resolveExternalTriggering(ADC_DMA *ADCPointer) {
    switch (LL_ADC_REG_GetTriggerSource(ADCPointer->ADCx)) {
        case LL_ADC_REG_TRIG_EXT_TIM2_TRGO:
        case LL_ADC_REG_TRIG_EXT_TIM3_TRGO:
        case LL_ADC_REG_TRIG_EXT_TIM8_TRGO: {
            uint32_t triggerEdge = LL_ADC_REG_GetTriggerEdge(ADCPointer->ADCx);
            LL_ADC_REG_StartConversionExtTrig(ADCPointer->ADCx, triggerEdge);
            break;
        }
    }
}