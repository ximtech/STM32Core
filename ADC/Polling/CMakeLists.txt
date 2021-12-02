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
        ${ADC_SOURCE_DIR}/ADC
        ${ADC_Polling_SOURCE_DIR}/include
        CACHE STRING "Polling ADC directories include to the main project")

set(ADC_POLLING_SOURCES
        ${DWT_DELAY_SOURCES}
        ${ADC_SOURCE_DIR}/ADC/ADCBase.c
        ${ADC_Polling_SOURCE_DIR}/${PROJECT_NAME}.c
        ${ADC_Polling_SOURCE_DIR}/include/${PROJECT_NAME}.h
        CACHE STRING "Polling ADC source files include to the main project")

set(SOURCE_FILES ${PROJECT_NAME}.c include/${PROJECT_NAME}.h)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

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