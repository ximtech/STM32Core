cmake_minimum_required(VERSION 3.20)
project(I2C_IT VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME RingBuffer
        GITHUB_REPOSITORY ximtech/RingBuffer
        GIT_TAG origin/main)

set(I2C_IT_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${I2C_IT_SOURCE_DIR}
        ${STM32_CORE_SOURCE_DIR}/I2C
        ${I2C_IT_SOURCE_DIR}/include
        CACHE STRING "IT I2C directories include to the main project" FORCE)

set(I2C_IT_SOURCES
        ${DWT_DELAY_SOURCES}
        ${STM32_CORE_SOURCE_DIR}/I2C/I2CBase.h
        ${I2C_IT_SOURCE_DIR}/${PROJECT_NAME}.c
        ${I2C_IT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "IT I2C source files include to the main project" FORCE)