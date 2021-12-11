cmake_minimum_required(VERSION 3.20)
project(ADC_DMA VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME DWTDelay
        GITHUB_REPOSITORY ximtech/DWTDelay
        GIT_TAG origin/main)

CPMAddPackage(
        NAME Vector
        GITHUB_REPOSITORY ximtech/Vector
        GIT_TAG origin/main)

set(DMA_UTILS_DIR ${STM32_CORE_SOURCE_DIR}/DMA)

set(ADC_DMA_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${DMA_UTILS_DIR}
        ${STM32_CORE_SOURCE_DIR}/ADC
        ${ADC_DMA_SOURCE_DIR}
        ${ADC_DMA_SOURCE_DIR}/include
        CACHE STRING "DMA ADC directories include to the main project" FORCE)

set(ADC_DMA_SOURCES
        ${DWT_DELAY_SOURCES}
        ${DMA_UTILS_DIR}/DMAUtils.c
        ${DMA_UTILS_DIR}/DMAUtils.h
        ${STM32_CORE_SOURCE_DIR}/ADC/ADCBase.h
        ${ADC_DMA_SOURCE_DIR}/${PROJECT_NAME}.c
        ${ADC_DMA_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "DMA ADC source files include to the main project" FORCE)