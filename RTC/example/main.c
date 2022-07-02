/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "HD44780_LCD_I2C.h"

#include "RTC.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static volatile bool isAlarmATriggered = false;
static volatile bool isAlarmBTriggered = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */




void alarmAEventCallback(RTC_TypeDef *RTCx) {
    isAlarmATriggered = true;
}

void alarmBEventCallback(RTC_TypeDef *RTCx) {
    isAlarmBTriggered = true;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_RTC_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    initLCD(I2C1);

    char timeBuffer[LCD_ROW_LENGTH] = {0};
    char dateBuffer[LCD_ROW_LENGTH] = {0};
    DateTimeFormatter dateFormatter;
    parseDateTimePattern(&dateFormatter, "yyyy-MM-dd");
    DateTimeFormatter timeFormatter;
    parseDateTimePattern(&timeFormatter, "HH:mm:ss");

    setRTCTime(23, 15, 35);
    setRTCDate(2022, JUNE, 27, THURSDAY);
    setRTCAlarm_A(27, 23, 15, 40, LL_RTC_ALMA_MASK_NONE);
    setRTCAlarm_B(27, 23, 16, 0, LL_RTC_ALMA_MASK_NONE);

//    LL_RTC_EnableIT_WUT(RTC);

/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE */
    while (1) {

        Time time = timeNow();
        Date date = dateNow();
        formatDate(&date, dateBuffer, LCD_ROW_LENGTH, &dateFormatter);
        formatTime(&time, timeBuffer, LCD_ROW_LENGTH, &timeFormatter);

        cleanPrintAtPositionLCD(0, 2, timeBuffer);
        cleanPrintAtPositionLCD(1, 2, dateBuffer);
        delay_ms(500);

        if (isAlarmATriggered) {
            cleanPrintAtPositionLCD(0, 2, "Alarm A");
            cleanPrintAtPositionLCD(1, 2, "TRIGGERED!!");
            isAlarmATriggered = false;
            delay_ms(3000);
        }

        if (isAlarmBTriggered) {
            cleanPrintAtPositionLCD(0, 2, "Alarm B");
            cleanPrintAtPositionLCD(1, 2, "TRIGGERED!!");
            isAlarmBTriggered = false;
            delay_ms(3000);
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3) {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1) {

    }
    LL_PWR_EnableBkUpAccess();
    LL_RCC_LSE_Enable();

    /* Wait till LSE is ready */
    while (LL_RCC_LSE_IsReady() != 1) {

    }
    if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE) {
        LL_RCC_ForceBackupDomainReset();
        LL_RCC_ReleaseBackupDomainReset();
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    }
    LL_RCC_EnableRTC();
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_12, 96, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {

    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {

    }
    LL_Init1msTick(100000000);
    LL_SetSystemCoreClock(100000000);
    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void) {

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    LL_I2C_InitTypeDef I2C_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    /**I2C1 GPIO Configuration
    PB6   ------> I2C1_SCL
    PB7   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    /** I2C Initialization
    */
    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    LL_I2C_EnableClockStretching(I2C1);
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = 100000;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
    LL_I2C_SetOwnAddress2(I2C1, 0);
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void) {

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
    LL_RTC_DateTypeDef RTC_DateStruct = {0};
    LL_RTC_AlarmTypeDef RTC_AlarmStruct = {0};

    /* Peripheral clock enable */
    LL_RCC_EnableRTC();

    /* RTC interrupt Init */
    NVIC_SetPriority(RTC_Alarm_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(RTC_Alarm_IRQn);

    /* USER CODE BEGIN RTC_Init 1 */
    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC and set the Time and Date
    */
    RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
    RTC_InitStruct.AsynchPrescaler = 127;
    RTC_InitStruct.SynchPrescaler = 255;
    LL_RTC_Init(RTC, &RTC_InitStruct);
    LL_RTC_SetAsynchPrescaler(RTC, 127);
    LL_RTC_SetSynchPrescaler(RTC, 255);
    /** Initialize RTC and set the Time and Date
    */
    if (LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2) {

        RTC_TimeStruct.Hours = 19;
        RTC_TimeStruct.Minutes = 23;
        RTC_TimeStruct.Seconds = 0;
        LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);
        RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_THURSDAY;
        RTC_DateStruct.Month = LL_RTC_MONTH_JUNE;
        RTC_DateStruct.Year = 22;
        LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_DateStruct);
        LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, 0x32F2);
    }
    /** Initialize RTC and set the Time and Date
    */
    if (LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2) {

        LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, 0x32F2);
    }
    /** Enable the Alarm A
    */
//  RTC_AlarmStruct.AlarmTime.Hours = 23;
//  RTC_AlarmStruct.AlarmTime.Minutes = 15;
//  RTC_AlarmStruct.AlarmTime.Seconds = 40;
//  RTC_AlarmStruct.AlarmMask = LL_RTC_ALMA_MASK_NONE;
//  RTC_AlarmStruct.AlarmDateWeekDaySel = LL_RTC_ALMA_DATEWEEKDAYSEL_DATE;
//  RTC_AlarmStruct.AlarmDateWeekDay = 27;
//  LL_RTC_ALMA_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_AlarmStruct);
//  LL_RTC_EnableIT_ALRA(RTC);
    /** Initialize RTC and set the Time and Date
    */
    if (LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2) {

        LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, 0x32F2);
    }
    /** Enable the WakeUp
    */
    LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_DIV_16);
    LL_RTC_WAKEUP_SetAutoReload(RTC, 0);
    /* USER CODE BEGIN RTC_Init 2 */

//    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);

//    RTC_AlarmStruct.AlarmTime.Hours = 23;
//    RTC_AlarmStruct.AlarmTime.Minutes = 15;
//    RTC_AlarmStruct.AlarmTime.Seconds = 40;
//    RTC_AlarmStruct.AlarmMask = LL_RTC_ALMA_MASK_ALL;
//    RTC_AlarmStruct.AlarmDateWeekDaySel = LL_RTC_ALMA_DATEWEEKDAYSEL_DATE;
//    RTC_AlarmStruct.AlarmDateWeekDay = 27;
//    LL_RTC_ALMA_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_AlarmStruct);


    /*LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_ALMA_Disable(RTC);
    LL_RTC_ClearFlag_ALRA(RTC);

    while (LL_RTC_IsActiveFlag_ALRAW(RTC) == RESET);

    LL_RTC_ALMA_Enable(RTC);
    LL_RTC_EnableIT_ALRA(RTC);

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);

    LL_RTC_EnableWriteProtection(RTC);*/










//    LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR1, 0x32F2);

//    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
    /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
/* User can add his own implementation to report the file name and line number,
   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
