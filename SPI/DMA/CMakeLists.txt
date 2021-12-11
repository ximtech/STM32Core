cmake_minimum_required(VERSION 3.20)
project(SPI_DMA VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME Vector
        GITHUB_REPOSITORY ximtech/Vector
        GIT_TAG origin/main)

set(DMA_UTILS_DIR ${STM32_CORE_SOURCE_DIR}/DMA)

set(SPI_DMA_DIRECTORY
        ${DMA_UTILS_DIR}
        ${STM32_CORE_SOURCE_DIR}/SPI
        ${SPI_DMA_SOURCE_DIR}
        ${SPI_DMA_SOURCE_DIR}/include
        CACHE STRING "DMA SPI directories include to the main project" FORCE)

set(SPI_DMA_SOURCES
        ${DMA_UTILS_DIR}/DMAUtils.c
        ${DMA_UTILS_DIR}/DMAUtils.h
        ${SPI_DMA_SOURCE_DIR}/${PROJECT_NAME}.c
        ${SPI_DMA_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "DMA SPI source files include to the main project" FORCE)