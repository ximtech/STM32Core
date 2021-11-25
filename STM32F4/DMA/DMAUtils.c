#include "DMAUtils.h"

#define ARRAY_SIZE(array)  (sizeof(array) / sizeof((array)[0]))

static void clearFlagTC(DMA_TypeDef *DMAx, uint32_t stream);
static void clearFlagTE(DMA_TypeDef *DMAx, uint32_t stream);

static const uint32_t DMA_CHANNEL_ARRAY[] = {
        LL_DMA_STREAM_0,
        LL_DMA_STREAM_1,
        LL_DMA_STREAM_2,
        LL_DMA_STREAM_3,
        LL_DMA_STREAM_4,
        LL_DMA_STREAM_5,
        LL_DMA_STREAM_6,
        LL_DMA_STREAM_7
};

bool isTransferCompleteInterruptEnabledDMA(DMA_TypeDef *DMAx, uint32_t stream) {
    switch (stream) {
        case LL_DMA_STREAM_0:
            return LL_DMA_IsActiveFlag_TC0(DMAx);
        case LL_DMA_STREAM_1:
            return LL_DMA_IsActiveFlag_TC1(DMAx);
        case LL_DMA_STREAM_2:
            return LL_DMA_IsActiveFlag_TC2(DMAx);
        case LL_DMA_STREAM_3:
            return LL_DMA_IsActiveFlag_TC3(DMAx);
        case LL_DMA_STREAM_4:
            return LL_DMA_IsActiveFlag_TC4(DMAx);
        case LL_DMA_STREAM_5:
            return LL_DMA_IsActiveFlag_TC5(DMAx);
        case LL_DMA_STREAM_6:
            return LL_DMA_IsActiveFlag_TC6(DMAx);
        case LL_DMA_STREAM_7:
            return LL_DMA_IsActiveFlag_TC7(DMAx);
        default:
           return false;
    }
}

bool isTransferErrorInterruptEnabledDMA(DMA_TypeDef *DMAx, uint32_t stream) {
    switch (stream) {
        case LL_DMA_STREAM_0:
            return LL_DMA_IsActiveFlag_TE0(DMAx);
        case LL_DMA_STREAM_1:
            return LL_DMA_IsActiveFlag_TE1(DMAx);
        case LL_DMA_STREAM_2:
            return LL_DMA_IsActiveFlag_TE2(DMAx);
        case LL_DMA_STREAM_3:
            return LL_DMA_IsActiveFlag_TE3(DMAx);
        case LL_DMA_STREAM_4:
            return LL_DMA_IsActiveFlag_TE4(DMAx);
        case LL_DMA_STREAM_5:
            return LL_DMA_IsActiveFlag_TE5(DMAx);
        case LL_DMA_STREAM_6:
            return LL_DMA_IsActiveFlag_TE6(DMAx);
        case LL_DMA_STREAM_7:
            return LL_DMA_IsActiveFlag_TE7(DMAx);
        default:
            return false;
    }
}

void disableTransferCompleteInterruptDMA(DMA_TypeDef *DMAx, uint32_t stream) {
    if (stream == LL_DMA_STREAM_ALL) {
        for (uint32_t i = 0; i < ARRAY_SIZE(DMA_CHANNEL_ARRAY); i++) {
            clearFlagTC(DMAx, DMA_CHANNEL_ARRAY[i]);
        }
    } else {
        clearFlagTC(DMAx, stream);
    }
}

void disableTransferErrorInterruptDMA(DMA_TypeDef *DMAx, uint32_t stream) {
    if (stream == LL_DMA_STREAM_ALL) {
        for (uint32_t i = 0; i < ARRAY_SIZE(DMA_CHANNEL_ARRAY); i++) {
            clearFlagTE(DMAx, DMA_CHANNEL_ARRAY[i]);
        }
    } else {
        clearFlagTE(DMAx, stream);
    }
}

void enableDMAStream(DMA_TypeDef *DMAx, uint32_t stream) {
    LL_DMA_DisableStream(DMAx, stream);
    disableTransferCompleteInterruptDMA(DMAx, stream);
    disableTransferErrorInterruptDMA(DMAx, stream);

    LL_DMA_EnableIT_TC(DMAx, stream);
    LL_DMA_EnableIT_TE(DMAx, stream);

    disableTransferCompleteInterruptDMA(DMAx, stream);
    disableTransferErrorInterruptDMA(DMAx, stream);
}

static void clearFlagTC(DMA_TypeDef *DMAx, uint32_t stream) {
    switch (stream) {
        case LL_DMA_STREAM_0:
            LL_DMA_ClearFlag_TC0(DMAx);
            break;
        case LL_DMA_STREAM_1:
            LL_DMA_ClearFlag_TC1(DMAx);
            break;
        case LL_DMA_STREAM_2:
            LL_DMA_ClearFlag_TC2(DMAx);
            break;
        case LL_DMA_STREAM_3:
            LL_DMA_ClearFlag_TC3(DMAx);
            break;
        case LL_DMA_STREAM_4:
            LL_DMA_ClearFlag_TC4(DMAx);
            break;
        case LL_DMA_STREAM_5:
            LL_DMA_ClearFlag_TC5(DMAx);
            break;
        case LL_DMA_STREAM_6:
            LL_DMA_ClearFlag_TC6(DMAx);
            break;
        case LL_DMA_STREAM_7:
            LL_DMA_ClearFlag_TC7(DMAx);
            break;
        default:
            break;
    }
}

static void clearFlagTE(DMA_TypeDef *DMAx, uint32_t stream) {
    switch (stream) {
        case LL_DMA_STREAM_0:
            LL_DMA_ClearFlag_TE0(DMAx);
            break;
        case LL_DMA_STREAM_1:
            LL_DMA_ClearFlag_TE1(DMAx);
            break;
        case LL_DMA_STREAM_2:
            LL_DMA_ClearFlag_TE2(DMAx);
            break;
        case LL_DMA_STREAM_3:
            LL_DMA_ClearFlag_TE3(DMAx);
            break;
        case LL_DMA_STREAM_4:
            LL_DMA_ClearFlag_TE4(DMAx);
            break;
        case LL_DMA_STREAM_5:
            LL_DMA_ClearFlag_TE5(DMAx);
            break;
        case LL_DMA_STREAM_6:
            LL_DMA_ClearFlag_TE6(DMAx);
            break;
        case LL_DMA_STREAM_7:
            LL_DMA_ClearFlag_TE7(DMAx);
            break;
        default:
            break;
    }
}