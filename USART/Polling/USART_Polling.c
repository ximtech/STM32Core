#include "USART_Polling.h"


void sendByteUSART(USART_TypeDef *USARTTypeDef, uint8_t byte) {
    while (!LL_USART_IsActiveFlag_TXE(USARTTypeDef));
    LL_USART_TransmitData8(USARTTypeDef, byte);
}


void sendStringUSART(USART_TypeDef *USARTTypeDef, char *string) {
    for (uint16_t i = 0; string[i] != '\0'; i++) {
        sendByteUSART(USARTTypeDef, string[i]);
    }
}

void sendFormattedStringUSART(USART_TypeDef *USARTTypeDef, size_t bufferLength, char *format, ...) {
   char formatBuffer[bufferLength];
    va_list args;

    va_start(args, format);
    vsnprintf(formatBuffer, bufferLength, format, args);
    va_end(args);

    sendStringUSART(USARTTypeDef, formatBuffer);
}

uint8_t readByteUSART(USART_TypeDef *USARTTypeDef) {
    while (!LL_USART_IsActiveFlag_RXNE(USARTTypeDef));
    return LL_USART_ReceiveData8(USARTTypeDef);
}

void readStringUSART(USART_TypeDef *USARTTypeDef, char *charArray, size_t length) {
    for (size_t i = 0; i < length; i++) {		// get the byte and put it in array
        charArray[i] = readByteUSART(USARTTypeDef);
    }
}

void readStringUntilStopCharUSART(USART_TypeDef *USARTTypeDef, char *charArray, size_t length, char stopChar) {  //get n bytes or until stopChar is received
    for (size_t i = 0; i < length; i++) {                 // for n bytes
        char receivedChar = readByteUSART(USARTTypeDef);    // get the next byte
        if(receivedChar == stopChar) {			            // if the next byte is the stopping character, quit the loop
            break;
        }
        charArray[i] = receivedChar;
    }
}