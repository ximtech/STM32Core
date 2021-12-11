cmake_minimum_required(VERSION 3.20)
project(USART_Polling VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

set(USART_POLLING_DIRECTORY
        ${STM32_CORE_SOURCE_DIR}/USART
        ${USART_Polling_SOURCE_DIR}
        ${USART_Polling_SOURCE_DIR}/include
        CACHE STRING "Polling USART directories include to the main project" FORCE)

set(USART_POLLING_SOURCES
        ${USART_Polling_SOURCE_DIR}/${PROJECT_NAME}.c
        ${USART_Polling_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "Polling USART source files include to the main project" FORCE)