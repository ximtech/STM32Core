#include "USART_IT.h"

#define NUMBER_OF_USART_IT_INSTANCES 3

#define FIRST_USART_INSTANCE_INDEX  0
#define SECOND_USART_INSTANCE_INDEX 1
#define SIXTH_USART_INSTANCE_INDEX  2

static USART_IT USARTInstanceArray[NUMBER_OF_USART_IT_INSTANCES] = { [0 ... NUMBER_OF_USART_IT_INSTANCES - 1] = NULL };

static USART_IT *cacheUSARTInstance(USART_IT USARTInstance);
static void interruptCallbackHandler(USART_IT *USARTPointer);
static void rxInterruptCallbackUSART(USART_IT *USARTPointer);
static void txInterruptCallbackUSART(USART_IT *USARTPointer);
static void clearInterruptFlag(USART_IT *USARTPointer);


USART_IT *initBufferedUSART_IT(USART_TypeDef *USARTx, uint32_t rxBufferSize, uint32_t txBufferSize) {
	if (USARTx == NULL) return NULL;
    USART_IT USARTInstance = {0};
    USARTInstance.RxBuffer = getRingBufferInstance(rxBufferSize);
    USARTInstance.TxBuffer = getRingBufferInstance(txBufferSize);
    USARTInstance.USARTx = USARTx;
	
	if (USARTInstance.RxBuffer == NULL || USARTInstance.TxBuffer == NULL) {
        return NULL;
    }
    LL_USART_EnableIT_RXNE(USARTx);
    LL_USART_EnableIT_ERROR(USARTx);
    return cacheUSARTInstance(USARTInstance);
}

void interruptCallbackUSART1() {
    USART_IT *USARTInstancePointer = &USARTInstanceArray[FIRST_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void interruptCallbackUSART2() {
    USART_IT *USARTInstancePointer = &USARTInstanceArray[SECOND_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void interruptCallbackUSART6() {
    USART_IT *USARTInstancePointer = &USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void sendByteUSART_IT(USART_IT *USARTPointer, uint8_t byte) {
    while (isRingBufferFull(USARTPointer->TxBuffer));
    ringBufferAdd(USARTPointer->TxBuffer, byte);
    LL_USART_EnableIT_TXE(USARTPointer->USARTx);
}

void sendStringUSART_IT(USART_IT *USARTPointer, const char *string) {
    for (uint16_t i = 0; string[i] != '\0'; i++) {
        if (isRingBufferNotFull(USARTPointer->TxBuffer)) {
            ringBufferAdd(USARTPointer->TxBuffer, string[i]);
        } else {
            LL_USART_EnableIT_TXE(USARTPointer->USARTx);  // if string is bigger than buffer size enable Tx interrupt and wait until data is send
            while (isRingBufferFull(USARTPointer->TxBuffer));
            ringBufferAdd(USARTPointer->TxBuffer, string[i]);
        }
    }
    LL_USART_EnableIT_TXE(USARTPointer->USARTx);
}

void sendFormattedStringUSART_IT(USART_IT *USARTPointer, uint16_t bufferLength, char *format, ...) {
    char formatBuffer[bufferLength];
    va_list args;

    va_start(args, format);
    vsnprintf(formatBuffer, bufferLength, format, args);
    va_end(args);

    sendStringUSART_IT(USARTPointer, formatBuffer);
}

uint8_t readByteUSART_IT(USART_IT *USARTPointer) {
    return ringBufferGet(USARTPointer->RxBuffer);
}

void readStringUSART_IT(USART_IT *USARTPointer, char *charArray) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTx));    // wait for complete data receive
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer); i++) {
        charArray[i] = ringBufferGet(USARTPointer->RxBuffer);
    }
}

void readStringForLengthUSART_IT(USART_IT *USARTPointer, char *charArray, uint32_t length) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTx) && getRingBufferSize(USARTPointer->RxBuffer) < length);// wait for complete data receive and data length restriction
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer) && (i < length); i++) {
        charArray[i] = ringBufferGet(USARTPointer->RxBuffer);
    }
}

void readStringUntilStopCharUSART_IT(USART_IT *USARTPointer, char *charArray, char stopChar) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTx));    // wait for complete data receive
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer); i++) {// if the next byte is the stopping character, quit the loop
        uint8_t byte = ringBufferGet(USARTPointer->RxBuffer);
        if (byte == stopChar) break;
        charArray[i] = byte;
    }
}

void deleteUSART_IT(USART_IT *USARTPointer) {
	if (USARTPointer != NULL) {
		ringBufferDelete(USARTPointer->RxBuffer);
		ringBufferDelete(USARTPointer->TxBuffer);
		free(USARTPointer);
	}
}

static USART_IT *cacheUSARTInstance(USART_IT USARTInstance) {
    if (USARTInstance.USARTx == USART1) {
        USARTInstanceArray[FIRST_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[FIRST_USART_INSTANCE_INDEX];
    } else if (USARTInstance.USARTx == USART2) {
        USARTInstanceArray[SECOND_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[SECOND_USART_INSTANCE_INDEX];
    } else if (USARTInstance.USARTx == USART6) {
        USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX];
    }
    return NULL;
}

static void interruptCallbackHandler(USART_IT *USARTPointer) {
    if (LL_USART_IsActiveFlag_RXNE(USARTPointer->USARTx) && LL_USART_IsEnabledIT_RXNE(USARTPointer->USARTx)) {
        rxInterruptCallbackUSART(USARTPointer);
    } else if (LL_USART_IsActiveFlag_TXE(USARTPointer->USARTx) && LL_USART_IsEnabledIT_TXE(USARTPointer->USARTx)) {
        txInterruptCallbackUSART(USARTPointer);
    } else {
        clearInterruptFlag(USARTPointer);
    }
}

static void rxInterruptCallbackUSART(USART_IT *USARTPointer) {// received a byte ISR
    if (isRingBufferNotFull(USARTPointer->RxBuffer)) {		// when buffer overflow, doesn't overwrite non read data
        uint8_t byte = LL_USART_ReceiveData8(USARTPointer->USARTx);
        ringBufferAdd(USARTPointer->RxBuffer, byte);
    }
}

static void txInterruptCallbackUSART(USART_IT *USARTPointer) {
    if (isRingBufferNotEmpty(USARTPointer->TxBuffer)) {
        LL_USART_TransmitData8(USARTPointer->USARTx, ringBufferGet(USARTPointer->TxBuffer));
    } else {
        LL_USART_DisableIT_TXE(USARTPointer->USARTx);// tx buffer empty, disable interrupt
    }
}

static void clearInterruptFlag(USART_IT *USARTPointer) {
    if (LL_USART_IsActiveFlag_ORE(USARTPointer->USARTx)) {
        LL_USART_ClearFlag_ORE(USARTPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_FE(USARTPointer->USARTx)) {
        LL_USART_ClearFlag_FE(USARTPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_NE(USARTPointer->USARTx)) {
        LL_USART_ClearFlag_NE(USARTPointer->USARTx);
    } else if (LL_USART_IsActiveFlag_PE(USARTPointer->USARTx)) {
        LL_USART_ClearFlag_PE(USARTPointer->USARTx);
    }
}