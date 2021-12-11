cmake_minimum_required(VERSION 3.20)
project(SPI_Polling VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(SPI_POLLING_DIRECTORY
        ${STM32_CORE_SOURCE_DIR}/SPI
        ${SPI_Polling_SOURCE_DIR}
        ${SPI_Polling_SOURCE_DIR}/include
        CACHE STRING "Polling SPI directories include to the main project" FORCE)

set(SPI_POLLING_SOURCES
        ${SPI_Polling_SOURCE_DIR}/${PROJECT_NAME}.c
        ${SPI_Polling_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "Polling SPI source files include to the main project" FORCE)