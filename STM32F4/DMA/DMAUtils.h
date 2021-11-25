#pragma once

#include <stdbool.h>
#include "stm32f4xx_ll_dma.h"

bool isTransferCompleteInterruptEnabledDMA(DMA_TypeDef *DMAx, uint32_t stream);
bool isTransferErrorInterruptEnabledDMA(DMA_TypeDef *DMAx, uint32_t stream);

void disableTransferCompleteInterruptDMA(DMA_TypeDef *DMAx, uint32_t stream);
void disableTransferErrorInterruptDMA(DMA_TypeDef *DMAx, uint32_t stream);

void enableDMAStream(DMA_TypeDef *DMAx, uint32_t stream);