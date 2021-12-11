cmake_minimum_required(VERSION 3.20)
project(SPI_IT VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME RingBuffer
        GITHUB_REPOSITORY ximtech/RingBuffer
        GIT_TAG origin/main)

set(SPI_IT_DIRECTORY
        ${STM32_CORE_SOURCE_DIR}/SPI
        ${SPI_IT_SOURCE_DIR}
        ${SPI_IT_SOURCE_DIR}/include
        CACHE STRING "IT SPI directories include to the main project" FORCE)

set(SPI_IT_SOURCES
        ${SPI_IT_SOURCE_DIR}/${PROJECT_NAME}.c
        ${SPI_IT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "IT SPI source files include to the main project" FORCE)