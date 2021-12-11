cmake_minimum_required(VERSION 3.20)
project(USART_IT VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME RingBuffer
        GITHUB_REPOSITORY ximtech/RingBuffer
        GIT_TAG origin/main)

set(USART_IT_DIRECTORY
        ${STM32_CORE_SOURCE_DIR}/USART
        ${USART_IT_SOURCE_DIR}
        ${USART_IT_SOURCE_DIR}/include
        CACHE STRING "IT USART directories include to the main project" FORCE)

set(USART_IT_SOURCES
        ${USART_IT_SOURCE_DIR}/${PROJECT_NAME}.c
        ${USART_IT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "IT USART source files include to the main project" FORCE)