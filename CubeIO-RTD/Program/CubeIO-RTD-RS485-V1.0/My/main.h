/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
//#include "stm32l0xx_hal_adc.h"
//#include "stm32l0xx_hal_adc_ex.h"
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_iwdg.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_crc.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_hal_rtc.h"

/* Private includes ----------------------------------------------------------*/
#define ENTER_CRITICAL_SECTION()     	__set_PRIMASK(1);	//系统关全局中断
#define EXIT_CRITICAL_SECTION()      	__set_PRIMASK(0);	//系统开全局中断

#define ENABLE_IRQ()                  __enable_irq();
#define DISABLE_IRQ()                 __disable_irq();

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>//itoa()......库中没有   
#include <string.h> //memset() 

//这两个宏定义必须放在这里否则会有错误
#define COMMAND_QUESTIONBUF_LENGTH	    128
#define COMMAND_RESPONSEBUF_LENGTH	    128

#include "stm32_seq.h"
#include "stm32_timer.h"
#include "stm32_lpm.h"
#include "utilities_def.h"
#include "stm32_adv_trace.h"
#include "hal_eeprom.h"
#include "hal_gpio.h"
#include "eeprom.h"
#include "utils.h"
#include "stm32_systime.h"
#include "delay_utils.h"
#include "usart.h"
#include "eeprom.h"

/* Private defines -----------------------------------------------------------*/
#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
