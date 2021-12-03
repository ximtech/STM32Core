# STM32Core

**STM32** LL(Low Layer) peripheral libraries such as ADC, UART, SPI, I2C etc.

### Features

- Specifically designed for embedded applications
- Ultra lightweight
- Easy import to the project and use
- Full feature support
- No HAL dependencies

### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
        NAME STM32Core
        GITHUB_REPOSITORY ximtech/STM32Core
        GIT_TAG origin/main)
```

## ADC

An ADC (Analog-To-Digital) converter is an electronic circuit that takes in an analog voltage as input and converts it
into digital data, a value that represents the voltage level in binary code.

1. Start project with STM32CubeMX:
    * Configuration for Polling ADC:
        * [Regular channel](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/config_regular_channel.PNG)
        * [Regular channel manual select](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/config_regular_manual_channel_select.PNG)
        * [Injected channel](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/config_injected_channel.PNG)
        * [Injected channel scan mode](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/config_injected_scan_mode.PNG)
    * Configuration for Interrupt ADC:
        * [NVIC config](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config.PNG)
        * [Timer config](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_timer.PNG)
        * [Timer triggering](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_timer_triggering.PNG)
        * [Continuous mode](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_continuous_mode.PNG)
        * [Injected channel config](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_injected_channel.PNG)
        * [Injected channel manual scan mode](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_injected_manual_scan_mode.PNG)
        * [Injected channel timer scan mode](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/config_injected_tim_scan_mode.PNG)
    * Configuration for DMA ADC:
        * [DMA config](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/config_dma.PNG)
        * [Single conversion mode](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/config_single_conversion.PNG)
        * [Continuous conversion mode](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/config_continuous.PNG)
        * [Timer config](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/config_tim.PNG)
        * [Timer triggering conversion](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/config_adc_tim_trigger.PNG)

2. Select: Project Manager -> Advanced Settings -> ADC -> LL
3. Generate Code
4. Add sources to project:

```cmake
# add in CmakeLists_template.txt or directly to CmakeLists.txt
include_directories(${includes}
        ${ADC_POLLING_DIRECTORY}
        ${ADC_IT_DIRECTORY}
        ${ADC_DMA_DIRECTORY})

file(GLOB_RECURSE SOURCES ${sources}
        ${ADC_POLLING_SOURCES}
        ${ADC_IT_SOURCES}
        ${ADC_DMA_SOURCES})

add_executable($${PROJECT_NAME}.elf $${SOURCES} $${LINKER_SCRIPT})

# link libraries to project
# Polling ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/Polling)
target_link_libraries(${PROJECT_NAME}.elf ADC_Polling)

# Interrupt based ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/IT)
target_link_libraries(${PROJECT_NAME}.elf ADC_IT)

# DMA based ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/DMA)
target_link_libraries(${PROJECT_NAME}.elf ADC_DMA)
```

3. Then Build -> Rebuild Project

## Usage

1. Polling ADC code examples:
    * [Regular channel](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/example_regular.c)
    * [Regular channel manual select](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/example_regular_manual_channel_select.c)
    * [Injected channel](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/example_injected.c)
    * [Injected channel scan mode](https://github.com/ximtech/STM32Core/blob/main/ADC/Polling/example/example_injected_scan.c)
2. Interrupt ADC code examples:
    * [Regular channel](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/example_regular.c)
    * [Injected channel manual switch](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/example_injected_manual.c)
    * [Injected channel scan](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/example_injected_manual_scan.c)
    * [Injected channel timer scan](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/example_injected_tim_scan.c)
    * [Injected channel timer trigger](https://github.com/ximtech/STM32Core/blob/main/ADC/IT/example/example_injected_tim_trigger.c)
4. DMA ADC code examples:
    * [Single conversion mode](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/example_software_conversion.c)
    * [Continuous conversion mode](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/example_continuous_conversion.c)
    * [Timer triggering conversion](https://github.com/ximtech/STM32Core/blob/main/ADC/DMA/example/example_tim_trigger.c)

For Interrupt and DMA ADC set handler functions in `stm32fXxx_it.c`
```c
/******************************************************************************/
/* STM32FXxx STM32 Peripheral Interrupt Handlers                              */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32fXxx.s).                    */
/******************************************************************************/

// This function handles ADC1 global interrupt.
void ADC_IRQHandler(void) {
    conventionCompleteCallbackADC(ADC1, ADC_REGULAR_CHANNEL);   // set ADC type and channel type
}

// This function handles DMA2 stream0 global interrupt.
void DMA2_Stream0_IRQHandler(void) {
    transferCompleteCallbackADC_DMA(DMA2, LL_DMA_STREAM_0); // set DMA type and stream
}
```

## In progress

- SPI
- UART
- I2C
