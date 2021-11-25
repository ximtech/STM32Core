#pragma once

#include "ADCBase.h"
#include <Vector.h>

typedef struct ADC_IT {
    ADC_TypeDef *ADCx;
    ADCChannelType type;
    ADCStatus status;
    uint32_t value;
    uint32_t rank;
} ADC_IT;

ADC_IT *initRegularADC_IT(ADC_TypeDef *ADCx);
ADC_IT *initInjectedADC_IT(ADC_TypeDef *ADCx, uint32_t rank);

ADCStatus startADC_IT(ADC_TypeDef *ADCx, ADCChannelType channelType);
void stopADC_IT(ADC_TypeDef *ADCx);

void readConversionDataADC_IT(ADC_TypeDef *ADCx, ADCChannelType channelType);  // for software single read. No need to use for continuous conversion mode or external trigger
void conventionCompleteCallbackADC(ADC_TypeDef *ADCx, ADCChannelType channelType);

double convertADCValueToVoltage_IT(ADC_IT *ADCPointer, double maxSourceVoltage);
void deleteADC_IT();