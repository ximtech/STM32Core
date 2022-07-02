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

# ADC

An ADC (Analog-To-Digital) converter is an electronic circuit that takes in an analog voltage as input and converts it
into digital data, a value that represents the voltage level in binary code.

### Project configuration

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
# link libraries to project
# Polling ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/Polling)
# Interrupt based ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/IT)
# DMA based ADC
add_subdirectory(${STM32_CORE_SOURCE_DIR}/ADC/DMA)

include_directories(${includes}
        ${ADC_POLLING_DIRECTORY}
        ${ADC_IT_DIRECTORY}
        ${ADC_DMA_DIRECTORY})

file(GLOB_RECURSE SOURCES ${sources}
        ${ADC_POLLING_SOURCES}
        ${ADC_IT_SOURCES}
        ${ADC_DMA_SOURCES})

target_link_libraries(${PROJECT_NAME}.elf Vector)   # add vector if ADC_IT or ADC_DMA is used
```

3. Then Build -> Clean -> Rebuild Project

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

# I2C
Inter-Integrated Circuit (I2C) is a communication bus protocol developed by Philips Semiconductor (now NXP Semiconductors) in 1982. 
It is a relatively slow protocol but has seen widespread use due to its simplicity and robustness.

### Project configuration

1. Start project with STM32CubeMX:
   * Configuration for Polling I2C:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/I2C/Polling/example/config.PNG)
   * Configuration for Interrupt I2C:
      * [Main settings](https://github.com/ximtech/STM32Core/blob/main/I2C/IT/example/config_1.PNG)
      * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/I2C/IT/example/config_2.PNG)
   * Configuration for DMA I2C:
      * [DMA config](https://github.com/ximtech/STM32Core/blob/main/I2C/DMA/example/config_1.PNG)

2. Select: Project Manager -> Advanced Settings -> I2C -> LL
3. Generate Code
4. Add sources to project:
```cmake
# add in CmakeLists_template.txt or directly to CmakeLists.txt
# link libraries
add_subdirectory(${STM32_CORE_SOURCE_DIR}/I2C/Polling)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/I2C/IT)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/I2C/DMA)

include_directories(${includes}
        ${I2C_POLLING_DIRECTORY}
        ${I2C_IT_DIRECTORY}
        ${I2C_DMA_DIRECTORY})

file(GLOB_RECURSE SOURCES ${sources}
        ${I2C_POLLING_SOURCES}
        ${I2C_IT_SOURCES}
        ${I2C_DMA_SOURCES})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT}) # executable declaration should be before libraries

target_link_libraries(${PROJECT_NAME}.elf RingBuffer)   # add ring buffer if I2C_IT is used
target_link_libraries(${PROJECT_NAME}.elf Vector)   # add vector if I2C_DMA is used
```

3. Then Build -> Clean -> Rebuild Project

## Usage
1. Polling I2C code examples:
   * [Find Device](https://github.com/ximtech/STM32Core/blob/main/I2C/Polling/example/example.c)
2. **TODO: add more examples**

# USART
The USART peripheral is used to interconnect STM32 MPU devices with other systems, typically via RS232 or RS485 protocols.

### Project configuration

1. Start project with STM32CubeMX:
   * Configuration for Polling USART:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/USART/Polling/example/config.PNG)
   * Configuration for Interrupt USART:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/USART/IT/example/config_1.PNG)
      * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/USART/IT/example/config_2.PNG)
   * Configuration for DMA USART:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/USART/DMA/example/config_1.PNG)
      * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/USART/DMA/example/config_2.PNG)
      * [DMA config](https://github.com/ximtech/STM32Core/blob/main/USART/DMA/example/config_3.PNG)

2. Select: Project Manager -> Advanced Settings -> USART -> LL
3. Generate Code
4. Add sources to project: 
```cmake
# add in CmakeLists_template.txt or directly to CmakeLists.txt
# link libraries
add_subdirectory(${STM32_CORE_SOURCE_DIR}/USART/DMA)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/USART/IT)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/USART/Polling)

include_directories(${includes}
        ${USART_POLLING_DIRECTORY}
        ${USART_IT_DIRECTORY}
        ${USART_DMA_DIRECTORY})

file(GLOB_RECURSE SOURCES ${sources}
        ${USART_POLLING_SOURCES}
        ${USART_IT_SOURCES}
        ${USART_DMA_SOURCES})

target_link_libraries(${PROJECT_NAME}.elf RingBuffer)   # add ring buffer if USART_IT is used
target_link_libraries(${PROJECT_NAME}.elf Vector)   # add vector if USART_DMA is used
```

3. Then Build -> Clean -> Rebuild Project

## Usage
1. Polling USART code example:
   * [Echo example](https://github.com/ximtech/STM32Core/blob/main/USART/Polling/example/example.c)
2. Interrupt USART code example:
   * [Echo example](https://github.com/ximtech/STM32Core/blob/main/USART/IT/example/example.c)
   * [Callback usage](https://github.com/ximtech/STM32Core/blob/main/USART/IT/example/stm32f4xx_it.c)
3. DMA USART code example:
   * [Echo example](https://github.com/ximtech/STM32Core/blob/main/USART/DMA/example/example.c)
   * [Callback usage](https://github.com/ximtech/STM32Core/blob/main/USART/DMA/example/stm32f4xx_it.c)

# SPI
SPI is an acronym for (Serial Peripheral Interface) pronounced as “S-P-I” or “Spy”. Which is an interface bus typically 
used for serial communication between microcomputer systems and other devices, memories, and sensors.

### Project configuration

1. Start project with STM32CubeMX:
   * Configuration for Polling SPI:
     * [Base config](https://github.com/ximtech/STM32Core/blob/main/SPI/IT/config/config_1.PNG)
   * Configuration for Interrupt SPI:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/SPI/IT/config/config_1.PNG)
      * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/SPI/IT/config/config_2.PNG)
   * Configuration for DMA SPI:
      * [Base config](https://github.com/ximtech/STM32Core/blob/main/SPI/DMA/config/config_2.PNG)
      * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/SPI/DMA/config/config_3.PNG)
      * [DMA config](https://github.com/ximtech/STM32Core/blob/main/SPI/DMA/config/config_1.PNG)

2. Select: Project Manager -> Advanced Settings -> SPI -> LL
3. Generate Code
4. Add sources to project: 
```cmake
# add in CmakeLists_template.txt or directly to CmakeLists.txt
# link libraries
add_subdirectory(${STM32_CORE_SOURCE_DIR}/SPI/Polling)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/SPI/IT)
add_subdirectory(${STM32_CORE_SOURCE_DIR}/SPI/DMA)

include_directories(${includes}
        ${SPI_POLLING_DIRECTORY}
        ${SPI_IT_DIRECTORY}
        ${SPI_DMA_DIRECTORY})

file(GLOB_RECURSE SOURCES ${sources}
        ${SPI_POLLING_SOURCES}
        ${SPI_IT_SOURCES}
        ${SPI_DMA_SOURCES})

target_link_libraries(${PROJECT_NAME}.elf RingBuffer)   # add ring buffer if SPI_IT is used
target_link_libraries(${PROJECT_NAME}.elf Vector)   # add vector if SPI_DMA is used
```

3. Then Build -> Clean -> Rebuild Project

## Usage
1. Polling SPI code example:
   * [Nokia LCD SPI Polling](https://github.com/ximtech/Nokia5110_LCD/blob/main/Nokia5110_LCD.c)
2. Interrupt SPI code example:
   * [Nokia LCD SPI IT](https://github.com/ximtech/Nokia5110_LCD/blob/main/Nokia5110_LCD_IT.c)
3. DMA SPI code example:
   * [Init example](https://github.com/ximtech/STM32Core/blob/main/SPI/DMA/config/initialization_example.PNG)
   * [Interrupt callback](https://github.com/ximtech/STM32Core/blob/main/SPI/DMA/config/interrupts_example.PNG)


# RTC
A real-time clock (RTC) is a computer clock that keeps track of the current time.

### Project configuration

1. Start project with STM32CubeMX:
    * Configuration for Polling SPI:
        * [RTC config 1](https://github.com/ximtech/STM32Core/blob/main/RTC/config/config_1.png)
        * [RTC config 2](https://github.com/ximtech/STM32Core/blob/main/RTC/config/config_2.png)
        * [NVIC settings](https://github.com/ximtech/STM32Core/blob/main/RTC/config/config_3.png)

2. Select: Project Manager -> Advanced Settings -> RTC -> LL
3. Generate Code
4. Add sources to project:
```cmake
# add in CmakeLists_template.txt or directly to CmakeLists.txt
# link libraries
CPMAddPackage(
        NAME GlobalDateTime
        GITHUB_REPOSITORY ximtech/GlobalDateTime
        GIT_TAG origin/main
        OPTIONS
        "ENABLE_TIME_ZONE_SUPPORT ON"
        "ENABLE_TIME_ZONE_HISTORIC_RULES OFF")

add_subdirectory(${STM32_CORE_SOURCE_DIR}/RTC)
include_directories(${includes} ${RTC_DIRECTORY})
file(GLOB_RECURSE SOURCES ${sources} ${RTC_SOURCES})

target_link_libraries(${PROJECT_NAME}.elf GlobalDateTime)   # library requires date-time library
```

3. Then Build -> Clean -> Rebuild Project

## Usage

1. RTC code example:
    * [Main](https://github.com/ximtech/STM32Core/blob/main/RTC/example/main.c)
2. Interrupt SPI code example:
    * [Alarm IT](https://github.com/ximtech/STM32Core/blob/main/RTC/example/stm32f4xx_it.c)
