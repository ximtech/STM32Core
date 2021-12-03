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
        ${STM32_CORE_SOURCE_DIR}/I2C
        ${I2C_IT_SOURCE_DIR}/include
        CACHE STRING "IT I2C directories include to the main project" FORCE)

set(I2C_IT_SOURCES
        ${DWT_DELAY_SOURCES}
        ${STM32_CORE_SOURCE_DIR}/I2C/I2CBase.h
        ${I2C_IT_SOURCE_DIR}/${PROJECT_NAME}.c
        ${I2C_IT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "IT I2C source files include to the main project" FORCE)

set(SOURCE_FILES
        ${PROJECT_NAME}.c
        include/${PROJECT_NAME}.h
        ${STM32_CORE_SOURCE_DIR}/I2C/I2CBase.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} RingBuffer)
target_link_libraries(${PROJECT_NAME} DWT_Delay)

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