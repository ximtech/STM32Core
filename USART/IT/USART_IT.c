#include "USART_IT.h"

#define NUMBER_OF_USART_INSTANCES 3

#define FIRST_USART_INSTANCE_INDEX  0
#define SECOND_USART_INSTANCE_INDEX 1
#define SIXTH_USART_INSTANCE_INDEX  2

static USART USARTInstanceArray[NUMBER_OF_USART_INSTANCES] = { [0 ... NUMBER_OF_USART_INSTANCES - 1] = NULL };

static USART *cacheUSARTInstance(USART USARTInstance);
static void interruptCallbackHandler(USART *USARTPointer);
static void rxInterruptCallbackUSART(USART *USARTPointer);
static void txInterruptCallbackUSART(USART *USARTPointer);
static void clearInterruptFlag(USART *USARTPointer);


USART *initBufferedUSART(USART_TypeDef *USARTTypeDef, uint32_t rxBufferSize, uint32_t txBufferSize) {
	if (USARTx == NULL) return NULL;
    USART USARTInstance = {0};
    USARTInstance.RxBuffer = getRingBufferInstance(rxBufferSize);
    USARTInstance.TxBuffer = getRingBufferInstance(txBufferSize);
    USARTInstance.USARTTypeDef = USARTTypeDef;
	
	if (USARTInstance.RxBuffer == NULL || USARTInstance.TxBuffer == NULL) {
        return NULL;
    }
    LL_USART_EnableIT_RXNE(USARTTypeDef);
    LL_USART_EnableIT_ERROR(USARTTypeDef);
    return cacheUSARTInstance(USARTInstance);
}

void interruptCallbackUSART1() {
    USART *USARTInstancePointer = &USARTInstanceArray[FIRST_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void interruptCallbackUSART2() {
    USART *USARTInstancePointer = &USARTInstanceArray[SECOND_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void interruptCallbackUSART6() {
    USART *USARTInstancePointer = &USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX];
    interruptCallbackHandler(USARTInstancePointer);
}

void sendByteUSART(USART *USARTPointer, uint8_t byte) {
    while (isRingBufferFull(USARTPointer->TxBuffer));
    ringBufferAddByte(USARTPointer->TxBuffer, byte);
    LL_USART_EnableIT_TXE(USARTPointer->USARTTypeDef);
}

void sendStringUSART(USART *USARTPointer, const char *string) {
    for (uint16_t i = 0; string[i] != '\0'; i++) {
        if (isRingBufferNotFull(USARTPointer->TxBuffer)) {
            ringBufferAddByte(USARTPointer->TxBuffer, string[i]);
        } else {
            LL_USART_EnableIT_TXE(USARTPointer->USARTTypeDef);  // if string is bigger than buffer size enable Tx interrupt and wait until data is send
            while (isRingBufferFull(USARTPointer->TxBuffer));
            ringBufferAddByte(USARTPointer->TxBuffer, string[i]);
        }
    }
    LL_USART_EnableIT_TXE(USARTPointer->USARTTypeDef);
}

void sendFormattedStringUSART(USART *USARTPointer, uint16_t bufferLength, char *format, ...) {
    char formatBuffer[bufferLength];
    va_list args;

    va_start(args, format);
    vsnprintf(formatBuffer, bufferLength, format, args);
    va_end(args);

    sendStringUSART(USARTPointer, formatBuffer);
}

uint8_t readByteUSART(USART *USARTPointer) {
    return ringBufferGetByte(USARTPointer->RxBuffer);
}

void readStringUSART(USART *USARTPointer, char *charArray) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTTypeDef));    // wait for complete data receive
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer); i++) {
        charArray[i] = ringBufferGet(USARTPointer->RxBuffer);
    }
}

void readStringForLengthUSART(USART *USARTPointer, char *charArray, uint32_t length) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTTypeDef) && getSize(USARTPointer->RxBuffer) < length);// wait for complete data receive and data length restriction
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer) && (i < length); i++) {
        charArray[i] = ringBufferGet(USARTPointer->RxBuffer);
    }
}

void readStringUntilStopCharUSART(USART *USARTPointer, char *charArray, char stopChar) {
    while (!LL_USART_IsActiveFlag_IDLE(USARTPointer->USARTTypeDef));    // wait for complete data receive
    for (uint16_t i = 0; isRingBufferNotEmpty(USARTPointer->RxBuffer) && (byte != stopChar); i++) {// if the next byte is the stopping character, quit the loop
        charArray[i] = ringBufferGet(USARTPointer->RxBuffer);
    }
}

void deleteUSART(USART *USARTPointer) {
	if (USARTPointer != NULL) {
		ringBufferDelete(USARTPointer->RxBuffer);
		ringBufferDelete(USARTPointer->TxBuffer);
		free(USARTPointer);
	}
}

static USART *cacheUSARTInstance(USART USARTInstance) {
    if (USARTInstance.USARTTypeDef == USART1) {
        USARTInstanceArray[FIRST_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[FIRST_USART_INSTANCE_INDEX];
    } else if (USARTInstance.USARTTypeDef == USART2) {
        USARTInstanceArray[SECOND_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[SECOND_USART_INSTANCE_INDEX];
    } else if (USARTInstance.USARTTypeDef == USART6) {
        USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX] = USARTInstance;
        return &USARTInstanceArray[SIXTH_USART_INSTANCE_INDEX];
    }
    return NULL;
}

static void interruptCallbackHandler(USART *USARTPointer) {
    if (LL_USART_IsActiveFlag_RXNE(USARTPointer->USARTTypeDef) && LL_USART_IsEnabledIT_RXNE(USARTPointer->USARTTypeDef)) {
        rxInterruptCallbackUSART(USARTPointer);
    } else if (LL_USART_IsActiveFlag_TXE(USARTPointer->USARTTypeDef) && LL_USART_IsEnabledIT_TXE(USARTPointer->USARTTypeDef)) {
        txInterruptCallbackUSART(USARTPointer);
    } else {
        clearInterruptFlag(USARTPointer);
    }
}

static void rxInterruptCallbackUSART(USART *USARTPointer) {// received a byte ISR
    if (isRingBufferNotFull(USARTPointer->RxBuffer)) {		// when buffer overflow, doesn't overwrite non read data
        uint8_t byte = LL_USART_ReceiveData8(USARTPointer->USARTTypeDef);
        ringBufferAdd(USARTPointer->RxBuffer, byte);
    }
}

static void txInterruptCallbackUSART(USART *USARTPointer) {
    if (isRingBufferNotEmpty(USARTPointer->TxBuffer)) {
        LL_USART_TransmitData8(USARTPointer->USARTTypeDef, ringBufferGet(USARTPointer->TxBuffer));
    } else {
        LL_USART_DisableIT_TXE(USARTPointer->USARTTypeDef);// tx buffer empty, disable interrupt
    }
}

static void clearInterruptFlag(USART *USARTPointer) {
    if (LL_USART_IsActiveFlag_ORE(USARTPointer->USARTTypeDef)) {
        LL_USART_ClearFlag_ORE(USARTPointer->USARTTypeDef);
    } else if (LL_USART_IsActiveFlag_FE(USARTPointer->USARTTypeDef)) {
        LL_USART_ClearFlag_FE(USARTPointer->USARTTypeDef);
    } else if (LL_USART_IsActiveFlag_NE(USARTPointer->USARTTypeDef)) {
        LL_USART_ClearFlag_NE(USARTPointer->USARTTypeDef);
    } else if (LL_USART_IsActiveFlag_PE(USARTPointer->USARTx)) {
        LL_USART_ClearFlag_PE(USARTPointer->USARTx);
    }
}