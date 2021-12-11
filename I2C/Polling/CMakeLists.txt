cmake_minimum_required(VERSION 3.20)
project(I2C_Polling VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME DWTDelay
        GITHUB_REPOSITORY ximtech/DWTDelay
        GIT_TAG origin/main)

set(I2C_POLLING_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${STM32_CORE_SOURCE_DIR}/I2C
        ${I2C_Polling_SOURCE_DIR}
        ${I2C_Polling_SOURCE_DIR}/include
        CACHE STRING "Polling I2C directories include to the main project" FORCE)

set(I2C_POLLING_SOURCES
        ${DWT_DELAY_SOURCES}
        ${STM32_CORE_SOURCE_DIR}/I2C/I2CBase.h
        ${I2C_Polling_SOURCE_DIR}/${PROJECT_NAME}.c
        ${I2C_Polling_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "Polling I2C source files include to the main project" FORCE)