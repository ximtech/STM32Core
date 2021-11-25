#include "ADC.h"

#define ADC_STABILIZATION_TIME_US 3

static ADCStatus readConversionDataRegularADC(ADC_TypeDef *ADCx);
static ADCStatus readConversionDataInjectedADC(ADC_TypeDef *ADCx);
static uint32_t getRegularADCValue(ADC_Polling *ADCPointer);
static uint32_t getInjectedADCValue(ADC_Polling *ADCPointer, uint32_t rank);

ADC_Polling initRegularPollingADC(ADC_TypeDef *ADCx, ADCChannel channel) {
    ADC_Polling adc = {0};
    adc.ADCx = ADCx;
    adc.type = ADC_REGULAR_CHANNEL;
    adc.regularRank = ADC_REG_RANK_1;
    adc.samplingTime = LL_ADC_SAMPLINGTIME_3CYCLES;
    adc.channel = channel;
    adc.value = 0;
    return adc;
}

ADC_Polling initInjectedPollingADC(ADC_TypeDef *ADCx, ADCChannel channel, ADCInjectedRank rank) {
    ADC_Polling adc = {0};
    adc.ADCx = ADCx;
    adc.type = ADC_INJECTED_CHANNEL;
    adc.samplingTime = LL_ADC_SAMPLINGTIME_3CYCLES;
    adc.channel = channel;
    adc.injectedRank = rank;
    adc.value = 0;
    return adc;
}

ADCStatus startADC(ADC_TypeDef *ADCx) {
    dwtDelayInit();
    LL_ADC_Enable(ADCx);
    delay_us(ADC_STABILIZATION_TIME_US);

    if (READ_BIT(ADCx->CR2, ADC_CR2_ADON) != ADC_CR2_ADON) {    // Start conversion if ADC is effectively enabled
        stopADC(ADCx);
        return ADC_INTERNAL_ERROR;
    } else {
        return ADC_OK;
    }
}

void stopADC(ADC_TypeDef *ADCx) {
    LL_ADC_Disable(ADCx);
}

void selectChannelADC(ADC_Polling *ADCPointer) {
    if (ADCPointer->type == ADC_REGULAR_CHANNEL) {
        LL_ADC_REG_SetSequencerRanks(ADCPointer->ADCx, ADCPointer->regularRank, ADCPointer->channel);
    } else {
        LL_ADC_INJ_SetSequencerRanks(ADCPointer->ADCx, ADCPointer->injectedRank, ADCPointer->channel);
    }
    LL_ADC_SetChannelSamplingTime(ADCPointer->ADCx, ADCPointer->channel, ADCPointer->samplingTime);
}

ADCStatus readConversionDataADC(ADC_TypeDef *ADCx, ADCChannelType channelType) {
    return (channelType == ADC_REGULAR_CHANNEL) ? readConversionDataRegularADC(ADCx) : readConversionDataInjectedADC(ADCx);
}

void getADCValue(ADC_Polling *ADCPointer) {
    if (ADCPointer->type == ADC_REGULAR_CHANNEL) {
        ADCPointer->value = getRegularADCValue(ADCPointer);
    } else {
        ADCPointer->value = getInjectedADCValue(ADCPointer, ADCPointer->injectedRank);
    }
}

double convertPollingADCValueToVoltage(ADC_Polling *ADCPointer, double maxSourceVoltage) {
    return convertADCValueToVoltage(ADCPointer->ADCx, ADCPointer->value, maxSourceVoltage);
}

static ADCStatus readConversionDataRegularADC(ADC_TypeDef *ADCx) {
    LL_ADC_REG_StartConversionSWStart(ADCx);

    uint32_t startTime = currentMilliSeconds();
    while (LL_ADC_IsActiveFlag_EOCS(ADCx) == RESET) {
        if ((currentMilliSeconds() - startTime) >= ADC_TIMEOUT_MS) {
            LL_ADC_ClearFlag_EOCS(ADCx);
            return ADC_READ_TIMEOUT_ERROR;
        }
    }
    LL_ADC_ClearFlag_EOCS(ADCx);
    return ADC_OK;
}

static ADCStatus readConversionDataInjectedADC(ADC_TypeDef *ADCx) {
    LL_ADC_INJ_StartConversionSWStart(ADCx);

    uint32_t startTime = currentMilliSeconds();
    while (LL_ADC_IsActiveFlag_JEOS(ADCx) == RESET) {
        if ((currentMilliSeconds() - startTime) >= ADC_TIMEOUT_MS) {
            LL_ADC_ClearFlag_JEOS(ADCx);
            return ADC_READ_TIMEOUT_ERROR;
        }
    }
    LL_ADC_ClearFlag_JEOS(ADCx);
    return ADC_OK;
}

static uint32_t getInjectedADCValue(ADC_Polling *ADCPointer, uint32_t rank) {
    switch (LL_ADC_GetResolution(ADCPointer->ADCx)) {
        case LL_ADC_RESOLUTION_12B:
            return LL_ADC_INJ_ReadConversionData12(ADCPointer->ADCx, rank);
        case LL_ADC_RESOLUTION_10B:
            return LL_ADC_INJ_ReadConversionData10(ADCPointer->ADCx, rank);
        case LL_ADC_RESOLUTION_8B:
            return LL_ADC_INJ_ReadConversionData8(ADCPointer->ADCx, rank);
        case LL_ADC_RESOLUTION_6B:
            return LL_ADC_INJ_ReadConversionData6(ADCPointer->ADCx, rank);
        default:
            return 0;
    }
}

static uint32_t getRegularADCValue(ADC_Polling *ADCPointer) {
    switch (LL_ADC_GetResolution(ADCPointer->ADCx)) {
        case LL_ADC_RESOLUTION_12B:
            return LL_ADC_REG_ReadConversionData12(ADCPointer->ADCx);
        case LL_ADC_RESOLUTION_10B:
            return LL_ADC_REG_ReadConversionData10(ADCPointer->ADCx);
        case LL_ADC_RESOLUTION_8B:
            return LL_ADC_REG_ReadConversionData8(ADCPointer->ADCx);
        case LL_ADC_RESOLUTION_6B:
            return LL_ADC_REG_ReadConversionData6(ADCPointer->ADCx);
        default:
            return 0;
    }
}