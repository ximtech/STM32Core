cmake_minimum_required(VERSION 3.20)

project(ADC)

option(ADC_POLLING "Enable building adc polling" OFF)
option(ADC_IT "Enable building adc it" OFF)

if(ADC_POLLING)
    add_subdirectory(ADC/Polling)
endif()
if(ADC_IT)
    add_subdirectory(ADC/IT)
endif()