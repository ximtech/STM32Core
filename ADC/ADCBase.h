#pragma once

#include "DWT_Delay.h"
#include "main.h"

typedef enum ADCStatus {
    ADC_OK,
    ADC_INTERNAL_ERROR, //ADC IP internal error: if problem of clocking, enable/disable, erroneous state
    ADC_READ_TIMEOUT_ERROR,
    ADC_OVERRUN_ERROR,
    ADC_DMA_ERROR       // DMA transfer error
} ADCStatus;

typedef enum ADCChannelType {
    ADC_REGULAR_CHANNEL,
    ADC_INJECTED_CHANNEL
} ADCChannelType;

typedef enum ADCChannel {
    ADC_CHANNEL_0 = LL_ADC_CHANNEL_0,
    ADC_CHANNEL_1 = LL_ADC_CHANNEL_1,
    ADC_CHANNEL_2 = LL_ADC_CHANNEL_2,
    ADC_CHANNEL_3 = LL_ADC_CHANNEL_3,
    ADC_CHANNEL_4 = LL_ADC_CHANNEL_4,
    ADC_CHANNEL_5 = LL_ADC_CHANNEL_5,
    ADC_CHANNEL_6 = LL_ADC_CHANNEL_6,
    ADC_CHANNEL_7 = LL_ADC_CHANNEL_7,
    ADC_CHANNEL_8 = LL_ADC_CHANNEL_8,
    ADC_CHANNEL_9 = LL_ADC_CHANNEL_9,
    ADC_CHANNEL_10 = LL_ADC_CHANNEL_10,
    ADC_CHANNEL_11 = LL_ADC_CHANNEL_11,
    ADC_CHANNEL_12 = LL_ADC_CHANNEL_12,
    ADC_CHANNEL_13 = LL_ADC_CHANNEL_13,
    ADC_CHANNEL_14 = LL_ADC_CHANNEL_14,
    ADC_CHANNEL_15 = LL_ADC_CHANNEL_15,
    ADC_CHANNEL_16 = LL_ADC_CHANNEL_16,
    ADC_CHANNEL_17 = LL_ADC_CHANNEL_17,
    ADC_CHANNEL_18 = LL_ADC_CHANNEL_18,
    ADC_CHANNEL_VREFINT = LL_ADC_CHANNEL_VREFINT,
    ADC_CHANNEL_VBAT = LL_ADC_CHANNEL_VBAT
} ADCChannel;

typedef enum ADCRegularRank {
    ADC_REG_RANK_1 = LL_ADC_REG_RANK_1,
    ADC_REG_RANK_2 = LL_ADC_REG_RANK_2,
    ADC_REG_RANK_3 = LL_ADC_REG_RANK_3,
    ADC_REG_RANK_4 = LL_ADC_REG_RANK_4,
    ADC_REG_RANK_5 = LL_ADC_REG_RANK_5,
    ADC_REG_RANK_6 = LL_ADC_REG_RANK_6,
    ADC_REG_RANK_7 = LL_ADC_REG_RANK_7,
    ADC_REG_RANK_8 = LL_ADC_REG_RANK_8,
    ADC_REG_RANK_9 = LL_ADC_REG_RANK_9,
    ADC_REG_RANK_10 = LL_ADC_REG_RANK_10,
    ADC_REG_RANK_11 = LL_ADC_REG_RANK_11,
    ADC_REG_RANK_12 = LL_ADC_REG_RANK_12,
    ADC_REG_RANK_13 = LL_ADC_REG_RANK_13,
    ADC_REG_RANK_14 = LL_ADC_REG_RANK_14,
    ADC_REG_RANK_15 = LL_ADC_REG_RANK_15,
    ADC_REG_RANK_16 = LL_ADC_REG_RANK_16
} ADCRegularRank;

typedef enum ADCInjectedRank {
    ADC_INJ_RANK_1 = LL_ADC_INJ_RANK_1,
    ADC_INJ_RANK_2 = LL_ADC_INJ_RANK_2,
    ADC_INJ_RANK_3 = LL_ADC_INJ_RANK_3,
    ADC_INJ_RANK_4 = LL_ADC_INJ_RANK_4
} ADCInjectedRank;

static inline double convertADCValueToVoltage(ADC_TypeDef *ADCx, uint32_t adcValue, double maxSourceVoltage) {
    double adcResolution = (double) __LL_ADC_DIGITAL_SCALE(LL_ADC_GetResolution(ADCx));
    return (adcValue * maxSourceVoltage) / adcResolution;
}