cmake_minimum_required(VERSION 3.20)
project(ADC_Polling VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

CPMAddPackage(
        NAME DWTDelay
        GITHUB_REPOSITORY ximtech/DWTDelay
        GIT_TAG origin/main)

set(ADC_POLLING_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${STM32_CORE_SOURCE_DIR}/ADC
        ${ADC_Polling_SOURCE_DIR}
        ${ADC_Polling_SOURCE_DIR}/include
        CACHE STRING "Polling ADC directories include to the main project" FORCE)

set(ADC_POLLING_SOURCES
        ${DWT_DELAY_SOURCES}
        ${STM32_CORE_SOURCE_DIR}/ADC/ADCBase.h
        ${ADC_Polling_SOURCE_DIR}/${PROJECT_NAME}.c
        ${ADC_Polling_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "Polling ADC source files include to the main project" FORCE)