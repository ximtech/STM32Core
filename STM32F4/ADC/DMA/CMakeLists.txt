cmake_minimum_required(VERSION 3.20)
project(ADC_DMA VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(../../../cmake/CPM.cmake)

CPMAddPackage(
        NAME DWTDelay
        GITHUB_REPOSITORY ximtech/DWTDelay
        GIT_TAG origin/main)

CPMAddPackage(
        NAME Vector
        GITHUB_REPOSITORY ximtech/Vector
        GIT_TAG origin/main)

set(SOURCE_FILES ADC_DMA.c ADC_DMA.h)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")


set(ADC_BASE_DIR ..)
set(DMA_UTILS_DIR ../../DMA)
target_include_directories(${PROJECT_NAME} PUBLIC ${ADC_BASE_DIR} ${DMA_UTILS_DIR})

target_link_libraries(${PROJECT_NAME} DWTDelay)
target_link_libraries(${PROJECT_NAME} Vector)