/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_lpm_if.c
  * @author  MCD Application Team
  * @brief   Low layer function to enter/exit low power modes (stop, sleep)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sys_debug.h"
#include "stm32_lpm_if.h"
#include "globals.h"
#include "hal_sdi12.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Power driver callbacks handler
  */
const struct UTIL_LPM_Driver_s UTIL_PowerDriver =
{
    PWR_EnterSleepMode,
    PWR_ExitSleepMode,

    PWR_EnterStopMode,
    PWR_ExitStopMode,

    PWR_EnterOffMode,
    PWR_ExitOffMode,
};

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void PWR_EnterOffMode(void)
{
    /* USER CODE BEGIN EnterOffMode_1 */

    /* USER CODE END EnterOffMode_1 */
}

void PWR_ExitOffMode(void)
{
    /* USER CODE BEGIN ExitOffMode_1 */

    /* USER CODE END ExitOffMode_1 */
}

void PWR_EnterStopMode(void)
{
    //设置唤醒后的默认时钟信号为HSI
    LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_HSI);

    UTILS_ENTER_CRITICAL_SECTION();

		//PIN_LEDPWR_ON();
    //必须先禁止中断，否则意外中断会造成问题
    SDI12_TXPIN_AS_EXTI_DISABLE();

    //准备进入STOP模式
    //重新设置变量
    g_sdiStateMachine = SDI_STANDBY;

    //禁用SDI12相关串口与IO
    Sdi12DeInit();

    //禁用ADC,IO
    MX_GPIO_EnterLowPower();

    //停止定时器
    MX_TIM2_DeInit();
    //MX_TIM21_DeInit();
    //MX_TIM22_Delay_DeInit();//这里如果调用的话，则传感器连续测量模式的I2C接口中的定时会不好用，所以不在这里做反向初始化

    //设置休眠时的内部电压调节器级别为LL_PWR_REGU_VOLTAGE_SCALE1
    //STM32L0 ERRATA P8 :Unexpected system reset when waking up from Stop mode with regulator in low-power mode
    //Workaround : Set the internal voltage regulator to Range1 before entering Stop mode.
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    //清除看门狗复位，清除RTC中断标志，使能RTC，保证休眠时可以进行看门狗复位操作
    LL_RTC_ClearFlag_WUT(RTC);

    //清除所有挂起的中断标志位，然后再开中断
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_ALL);
    SDI12_TXPIN_AS_EXTI_ENABLE();
    PIN_LEDPWR_OFF();

    /*clear wake up flag*/
    SET_BIT(PWR->CR, PWR_CR_CWUF);

    UTILS_EXIT_CRITICAL_SECTION();

    /* Enter Stop Mode */
    //重要：单片机BUG瑕疵<<stm32l031x46 Errata-device-limitations.pdf>> Page 10,
    //I2C and USART cannot wake up the device from Stop mode
    //Two solutions are then possible to perform I2C or USART communications:
    //? Put the microcontroller in a mode different from Stop (or Standby mode) before
    //initiating communications.
    //? Replace Stop mode with Stop mode plus regulator in main mode by keeping LPSDSR
    //bit of PWR_CR set to ‘0’.
    //HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
}

//这个函数是被中断唤醒后执行的第一个函数，执行完后再执行相应的中断响应
void PWR_ExitStopMode(void)
{
    /* Disable IRQ while the MCU is not running on HSI */

    UTILS_ENTER_CRITICAL_SECTION();

    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSI */
    __HAL_RCC_HSI_ENABLE();

    // Wait till HSI is ready
    //while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET) {}

    //Select HSI as system clock source
    //__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);

    // Wait till HSI is used as system clock source
    //while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_HSI) {}

    /* initializes the peripherals */
    MX_GPIO_ExitLowPower();
		//MX_TIM22_Delay_Init();

    UTILS_EXIT_CRITICAL_SECTION();
}

void PWR_EnterSleepMode(void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void PWR_ExitSleepMode(void)
{
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
