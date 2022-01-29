#include "ADC_IT.h"

#define INITIAL_NUMBER_OF_ADC_IT_INSTANCES 1
#define ADC_STABILIZATION_TIME_US 3

static Vector ADCInstanceCache = NULL;

static void startRegularChannelConversion(ADC_TypeDef *ADCx);
static void startInjectedChannelConversion(ADC_TypeDef *ADCx);

static void resolveRegularChannelExternalTriggering(ADC_TypeDef *ADCx);
static void resolveInjectedChannelExternalTriggering(ADC_TypeDef *ADCx);

static void interruptHandlerForRegularChannelADC(ADC_TypeDef *ADCx);
static void interruptHandlerForInjectedChannelADC(ADC_TypeDef *ADCx);

static void setRegularChannelADCValue(ADC_IT *ADCPointer);
void setInjectedChannelADCValue(ADC_IT *ADCPointer, uint32_t rank);


ADC_IT *initRegularADC_IT(ADC_TypeDef *ADCx, uint32_t rank, uint32_t channel) {
    ADC_IT *ADCPointer = malloc(sizeof(ADC_IT));
    if (ADCPointer == NULL) return NULL;
    ADCPointer->ADCx = ADCx;
    ADCPointer->type = ADC_REGULAR_CHANNEL;
    ADCPointer->status = ADC_OK;
    ADCPointer->channel = __LL_ADC_CHANNEL_TO_DECIMAL_NB(channel);
    ADCPointer->rank = rank;
    ADCPointer->value = 0;

	initSingletonVector(&ADCInstanceCache, INITIAL_NUMBER_OF_ADC_IT_INSTANCES);
    vectorAdd(ADCInstanceCache, ADCPointer);
    return ADCPointer;
}

ADC_IT *initInjectedADC_IT(ADC_TypeDef *ADCx, uint32_t rank) {
    if (rank < 1) return NULL;
    ADC_IT *ADCPointer = initRegularADC_IT(ADCx, rank, 0);
    if (ADCPointer == NULL) return NULL;
    ADCPointer->type = ADC_INJECTED_CHANNEL;
    return ADCPointer;
}

ADCStatus startADC_IT(ADC_TypeDef *ADCx, ADCChannelType channelType) {
    LL_ADC_Enable(ADCx);
    delay_us(ADC_STABILIZATION_TIME_US);

    if (READ_BIT(ADCx->CR2, ADC_CR2_ADON) != ADC_CR2_ADON) {    // Start conversion if ADC is effectively enabled
        stopADC_IT(ADCx);
        return ADC_INTERNAL_ERROR;
    }

    if (channelType == ADC_REGULAR_CHANNEL) {
        LL_ADC_EnableIT_EOCS(ADCx);
        startRegularChannelConversion(ADCx);
    } else {
        LL_ADC_EnableIT_JEOS(ADCx);
        startInjectedChannelConversion(ADCx);
    }
    LL_ADC_EnableIT_OVR(ADCx);
    return ADC_OK;
}

void selectChannelADC_IT(ADC_IT *ADCPointer) {
    if (ADCPointer->type == ADC_REGULAR_CHANNEL) {
        LL_ADC_REG_SetSequencerRanks(ADCPointer->ADCx, ADCPointer->rank, ADCPointer->channel);
    } else {
        LL_ADC_INJ_SetSequencerRanks(ADCPointer->ADCx, ADCPointer->rank, ADCPointer->channel);
    }
    LL_ADC_SetChannelSamplingTime(ADCPointer->ADCx, ADCPointer->channel, LL_ADC_GetChannelSamplingTime(ADCPointer->ADCx, ADCPointer->channel));
}

void stopADC_IT(ADC_TypeDef *ADCx) {
    LL_ADC_DisableIT_EOCS(ADCx);
    LL_ADC_DisableIT_JEOS(ADCx);
    LL_ADC_DisableIT_OVR(ADCx);
    LL_ADC_Disable(ADCx);
}

void conventionCompleteCallbackADC(ADC_TypeDef *ADCx, ADCChannelType channelType) {
    if (channelType == ADC_REGULAR_CHANNEL) {
        interruptHandlerForRegularChannelADC(ADCx);
    } else {
        interruptHandlerForInjectedChannelADC(ADCx);
    }
}

static void interruptHandlerForRegularChannelADC(ADC_TypeDef *ADCx) {
    for (uint32_t i = 0; i < getVectorSize(ADCInstanceCache); i++) {
        ADC_IT *ADCPointer = vectorGet(ADCInstanceCache, i);

        if (ADCPointer != NULL && ADCPointer->ADCx == ADCx && ADCPointer->type == ADC_REGULAR_CHANNEL) {
            uint32_t channel = LL_ADC_REG_GetSequencerRanks(ADCPointer->ADCx, ADCPointer->rank);
            if (ADCPointer->channel == channel) {
                if (LL_ADC_IsActiveFlag_EOCS(ADCPointer->ADCx)) {
                    LL_ADC_ClearFlag_EOCS(ADCPointer->ADCx);
                    setRegularChannelADCValue(ADCPointer);
                    ADCPointer->status = ADC_OK;
                } else if (LL_ADC_IsActiveFlag_OVR(ADCPointer->ADCx)) {
                    LL_ADC_ClearFlag_OVR(ADCPointer->ADCx);
                    ADCPointer->status = ADC_OVERRUN_ERROR;
                }
                break;
            }
        }
    }
}

static void interruptHandlerForInjectedChannelADC(ADC_TypeDef *ADCx) {
    if (LL_ADC_IsActiveFlag_JEOS(ADCx)) {
        LL_ADC_ClearFlag_JEOS(ADCx);
        for (uint32_t i = 0; i < getVectorSize(ADCInstanceCache); i++) {
            ADC_IT *ADCPointer = vectorGet(ADCInstanceCache, i);
            if (ADCPointer != NULL && ADCPointer->ADCx == ADCx && ADCPointer->type == ADC_INJECTED_CHANNEL) {
                setInjectedChannelADCValue(ADCPointer, ADCPointer->rank);
                ADCPointer->status = ADC_OK;
            }
        }
    } else if (LL_ADC_IsActiveFlag_OVR(ADCx)) {
        LL_ADC_ClearFlag_OVR(ADCx);
        for (uint32_t i = 0; i < getVectorSize(ADCInstanceCache); i++) {
            ADC_IT *ADCPointer = vectorGet(ADCInstanceCache, i);
            if (ADCPointer != NULL && ADCPointer->ADCx == ADCx && ADCPointer->type == ADC_INJECTED_CHANNEL) {
                ADCPointer->status = ADC_OVERRUN_ERROR;
            }
        }
    }
}

void readConversionDataADC_IT(ADC_TypeDef *ADCx, ADCChannelType channelType) {
    if (channelType == ADC_REGULAR_CHANNEL) {
        if (LL_ADC_REG_GetContinuousMode(ADCx) == LL_ADC_REG_CONV_SINGLE) { // call if single measure mode is enabled
            LL_ADC_REG_StartConversionSWStart(ADCx);
        }
    } else {
        if (LL_ADC_REG_GetContinuousMode(ADCx) == LL_ADC_REG_CONV_SINGLE) { // call if single measure mode is enabled
            LL_ADC_INJ_StartConversionSWStart(ADCx);
        }
    }
}

double convertADCValueToVoltage_IT(ADC_IT *ADCPointer, double maxSourceVoltage) {
    return convertADCValueToVoltage(ADCPointer->ADCx, ADCPointer->value, maxSourceVoltage);
}

void deleteADC_IT() {
    for (uint32_t i = 0; i < getVectorSize(ADCInstanceCache); i++) {
        free(vectorGet(ADCInstanceCache, i));
    }
    vectorDelete(ADCInstanceCache);
    ADCInstanceCache = NULL;
}

static void startRegularChannelConversion(ADC_TypeDef *ADCx) {
    if (LL_ADC_REG_IsTriggerSourceSWStart(ADCx)) {  // software manual triggering
        LL_ADC_REG_StartConversionSWStart(ADCx);    // initial measure, start continuous conversion mode if enabled
        delay_ms(100);
    } else {
        resolveRegularChannelExternalTriggering(ADCx);
    }
}

static void startInjectedChannelConversion(ADC_TypeDef *ADCx) {
    if (LL_ADC_INJ_IsTriggerSourceSWStart(ADCx)) {  // software manual triggering
        LL_ADC_INJ_StartConversionSWStart(ADCx);    // initial measure, start continuous conversion mode if enabled
        delay_ms(100);
    } else {
        resolveInjectedChannelExternalTriggering(ADCx);
    }
}

static void resolveRegularChannelExternalTriggering(ADC_TypeDef *ADCx) {
    switch (LL_ADC_REG_GetTriggerSource(ADCx)) {
        case LL_ADC_REG_TRIG_EXT_TIM2_TRGO:
        case LL_ADC_REG_TRIG_EXT_TIM3_TRGO:
        case LL_ADC_REG_TRIG_EXT_TIM8_TRGO: {
            uint32_t triggerEdge = LL_ADC_REG_GetTriggerEdge(ADCx);
            LL_ADC_REG_StartConversionExtTrig(ADCx, triggerEdge);
            break;
        }
    }
}

static void resolveInjectedChannelExternalTriggering(ADC_TypeDef *ADCx) {
    switch (LL_ADC_INJ_GetTriggerSource(ADCx)) {
        case LL_ADC_INJ_TRIG_EXT_TIM1_TRGO:
        case LL_ADC_INJ_TRIG_EXT_TIM2_TRGO:
        case LL_ADC_INJ_TRIG_EXT_TIM4_TRGO:
        case LL_ADC_INJ_TRIG_EXT_TIM5_TRGO: {
            uint32_t triggerEdge = LL_ADC_INJ_GetTriggerEdge(ADCx);
            LL_ADC_INJ_StartConversionExtTrig(ADCx, triggerEdge);
            break;
        }
    }
}

static void setRegularChannelADCValue(ADC_IT *ADCPointer) {
    switch (LL_ADC_GetResolution(ADCPointer->ADCx)) {
        case LL_ADC_RESOLUTION_12B:
            ADCPointer->value = LL_ADC_REG_ReadConversionData12(ADCPointer->ADCx);
            break;
        case LL_ADC_RESOLUTION_10B:
            ADCPointer->value = LL_ADC_REG_ReadConversionData10(ADCPointer->ADCx);
            break;
        case LL_ADC_RESOLUTION_8B:
            ADCPointer->value = LL_ADC_REG_ReadConversionData8(ADCPointer->ADCx);
            break;
        case LL_ADC_RESOLUTION_6B:
            ADCPointer->value = LL_ADC_REG_ReadConversionData6(ADCPointer->ADCx);
            break;
        default:
            ADCPointer->value = 0;
            break;
    }
}

void setInjectedChannelADCValue(ADC_IT *ADCPointer, uint32_t rank) {
    switch (LL_ADC_GetResolution(ADCPointer->ADCx)) {
        case LL_ADC_RESOLUTION_12B:
            ADCPointer->value = LL_ADC_INJ_ReadConversionData12(ADCPointer->ADCx, rank);
            break;
        case LL_ADC_RESOLUTION_10B:
            ADCPointer->value = LL_ADC_INJ_ReadConversionData10(ADCPointer->ADCx, rank);
            break;
        case LL_ADC_RESOLUTION_8B:
            ADCPointer->value = LL_ADC_INJ_ReadConversionData8(ADCPointer->ADCx, rank);
            break;
        case LL_ADC_RESOLUTION_6B:
            ADCPointer->value = LL_ADC_INJ_ReadConversionData6(ADCPointer->ADCx, rank);
            break;
        default:
            ADCPointer->value = 0;
            break;
    }
}
