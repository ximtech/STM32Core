#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "main.h"

// Note: USART_TypeDef *USARTTypeDef, should be already initialized
void sendByteUSART(USART_TypeDef *USARTTypeDef, uint8_t byte);
void sendStringUSART(USART_TypeDef *USARTTypeDef, char *string);
void sendFormattedStringUSART(USART_TypeDef *USARTTypeDef, size_t bufferLength, char *format, ...);

uint8_t readByteUSART(USART_TypeDef *USARTTypeDef);
void readStringUSART(USART_TypeDef *USARTTypeDef, char *charArray, size_t length);
void readStringUntilStopCharUSART(USART_TypeDef *USARTTypeDef, char *charArray, size_t length, char stopChar);