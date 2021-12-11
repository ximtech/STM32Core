cmake_minimum_required(VERSION 3.20)
project(I2C_DMA VERSION 1.0 LANGUAGES C)

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

set(I2C_DMA_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${DMA_UTILS_DIR}
        ${STM32_CORE_SOURCE_DIR}/I2C
        ${I2C_DMA_SOURCE_DIR}
        ${I2C_DMA_SOURCE_DIR}/include
        CACHE STRING "DMA I2C directories include to the main project" FORCE)

set(I2C_DMA_SOURCES
        ${DWT_DELAY_SOURCES}
        ${DMA_UTILS_DIR}/DMAUtils.c
        ${DMA_UTILS_DIR}/DMAUtils.h
        ${STM32_CORE_SOURCE_DIR}/I2C/I2CBase.h
        ${I2C_DMA_SOURCE_DIR}/${PROJECT_NAME}.c
        ${I2C_DMA_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "DMA I2C source files include to the main project" FORCE)