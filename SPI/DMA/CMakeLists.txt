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
        ${SPI_DMA_SOURCE_DIR}/include
        CACHE STRING "DMA SPI directories include to the main project" FORCE)

set(SPI_DMA_SOURCES
        ${DMA_UTILS_DIR}/DMAUtils.c
        ${DMA_UTILS_DIR}/DMAUtils.h
        ${SPI_DMA_SOURCE_DIR}/${PROJECT_NAME}.c
        ${SPI_DMA_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "DMA SPI source files include to the main project" FORCE)

set(SOURCE_FILES
        ${PROJECT_NAME}.c
        include/${PROJECT_NAME}.h
        ${DMA_UTILS_DIR}/DMAUtils.c
        ${DMA_UTILS_DIR}/DMAUtils.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} Vector)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Export
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Export
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})