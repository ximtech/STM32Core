cmake_minimum_required(VERSION 3.20)
project(RTC VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../cmake/CPM.cmake)

set(RTC_DIRECTORY
        ${STM32_CORE_SOURCE_DIR}/RTC
        ${STM32_CORE_SOURCE_DIR}/RTC/include
        CACHE STRING "RTC directories include to the main project" FORCE)

set(RTC_SOURCES
        ${STM32_CORE_SOURCE_DIR}/RTC/${PROJECT_NAME}.c
        ${STM32_CORE_SOURCE_DIR}/RTC/include/${PROJECT_NAME}.h
        CACHE STRING "RTC source files include to the main project" FORCE)