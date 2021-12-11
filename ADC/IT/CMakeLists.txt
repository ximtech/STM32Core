cmake_minimum_required(VERSION 3.20)
project(ADC_IT VERSION 1.0 LANGUAGES C)

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

set(ADC_IT_DIRECTORY
        ${DWT_DELAY_DIRECTORY}
        ${STM32_CORE_SOURCE_DIR}/ADC
        ${ADC_IT_SOURCE_DIR}
        ${ADC_IT_SOURCE_DIR}/include
        CACHE STRING "IT ADC directories include to the main project" FORCE)

set(ADC_IT_SOURCES
        ${DWT_DELAY_SOURCES}
        ${STM32_CORE_SOURCE_DIR}/ADC/ADCBase.h
        ${ADC_IT_SOURCE_DIR}/${PROJECT_NAME}.c
        ${ADC_IT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "IT ADC source files include to the main project" FORCE)