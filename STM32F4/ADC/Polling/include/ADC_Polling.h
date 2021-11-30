#pragma once

#include "ADCBase.h"

#define ADC_TIMEOUT_MS 1000

typedef struct ADC_Polling {
    ADC_TypeDef *ADCx;
    ADCChannelType type;
    ADCChannel channel;
    uint32_t samplingTime;  // by default set to minimum cycles
    uint32_t value;
    union {
        ADCRegularRank regularRank; // by default, first rank is selected
        ADCInjectedRank injectedRank;
    };
} ADC_Polling;

ADC_Polling initRegularPollingADC(ADC_TypeDef *ADCx, ADCChannel channel);
ADC_Polling initInjectedPollingADC(ADC_TypeDef *ADCx, ADCChannel channel, ADCInjectedRank rank);

ADCStatus startADC(ADC_TypeDef *ADCx);
void stopADC(ADC_TypeDef *ADCx);

void selectChannelADC(ADC_Polling *ADCPointer);

ADCStatus readConversionDataADC(ADC_TypeDef *ADCx, ADCChannelType channelType);
void getADCValue(ADC_Polling *ADCPointer);  // get value after read

double convertPollingADCValueToVoltage(ADC_Polling *ADCPointer, double maxSourceVoltage);