/*
 * gpio.h
 *
 *  Created on: 2010-5-2
 *      Author: Administrator
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "main.h"

//******************************************
// 现在不用，但以后可能会用到
//******************************************
//#define LEDPWR_PORT           GPIOB
//#define LEDPWR_PIN            LL_GPIO_PIN_1
#define PIN_LEDPWR_OFF()        //LL_GPIO_ResetOutputPin(LEDPWR_PORT,LEDPWR_PIN)
#define PIN_LEDPWR_ON()         //LL_GPIO_SetOutputPin(LEDPWR_PORT,LEDPWR_PIN)
#define PIN_LEDPWR_TOGGLE()     //LL_GPIO_TogglePin(LEDPWR_PORT,LEDPWR_PIN)

//******************************************
// GPIOA定义
// GPIOA.00.01.07.09.10不使用
//******************************************

// GPIOA.5.6作为I2C通信
#define I2C_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2C_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define I2C_PORT                GPIOA
#define I2C_SCL_PIN             LL_GPIO_PIN_9
#define I2C_SDA_PIN             LL_GPIO_PIN_8

#define I2C_SCL_HIGH            LL_GPIO_SetOutputPin(I2C_PORT,I2C_SCL_PIN)
#define I2C_SCL_LOW             LL_GPIO_ResetOutputPin(I2C_PORT,I2C_SCL_PIN)

#define I2C_SDA_HIGH            LL_GPIO_SetOutputPin(I2C_PORT,I2C_SDA_PIN)
#define I2C_SDA_LOW             LL_GPIO_ResetOutputPin(I2C_PORT,I2C_SDA_PIN)

#define I2C_SCL_STATE           LL_GPIO_IsInputPinSet(I2C_PORT,I2C_SCL_PIN)
#define I2C_SDA_STATE           LL_GPIO_IsInputPinSet(I2C_PORT,I2C_SDA_PIN)

#define I2C_SCL_OUTOD           LL_GPIO_SetOutputPin(I2C_PORT,I2C_SCL_PIN)
#define I2C_SDA_OUTOD           LL_GPIO_SetOutputPin(I2C_PORT,I2C_SDA_PIN)


// GPIOA.02.03作为SDI12通信接口，其中SDI12_TX(PA2)复用为SDI12的中断引脚
#define SDI12_TX_PORT        		GPIOA
#define SDI12_TX_PIN         	  LL_GPIO_PIN_2

#define SDI12_RX_PORT        		GPIOA
#define SDI12_RX_PIN         	  LL_GPIO_PIN_3

#define SDI12_TX_HIGH()         LL_GPIO_SetOutputPin(SDI12_TX_PORT,SDI12_TX_PIN)
#define SDI12_TX_LOW()          LL_GPIO_ResetOutputPin(SDI12_TX_PORT,SDI12_TX_PIN)

#define SDI12_TXPIN_AS_EXTI_LINE         LL_EXTI_LINE_2
#define SDI12_TXPIN_AS_EXTI_IRQn         EXTI2_3_IRQn
#define SDI12_TXPIN_AS_EXTI_ENABLE()     LL_EXTI_EnableIT_0_31(SDI12_TXPIN_AS_EXTI_LINE);
#define SDI12_TXPIN_AS_EXTI_DISABLE()    LL_EXTI_DisableIT_0_31(SDI12_TXPIN_AS_EXTI_LINE);

#define SDI12_TXDEN_PORT        GPIOA
#define SDI12_TXDEN_PIN         LL_GPIO_PIN_4
#define SDI12_RXDEN_OFF()       LL_GPIO_SetOutputPin(SDI12_TXDEN_PORT,SDI12_TXDEN_PIN)
#define SDI12_RXDEN_ON()        LL_GPIO_ResetOutputPin(SDI12_TXDEN_PORT,SDI12_TXDEN_PIN)

#define USART_SDI12 									USART2
#define RCC_APB2Periph_USART_SDI12		LL_APB1_GRP1_PERIPH_USART2
#define USART_SDI12_GPIO_ALTERNATIVE_FUNCTION  LL_GPIO_AF_4

#define DMA_FLAG_USART_SDI12_TX_GL    DMA1_FLAG_GL4
#define DMA_FLAG_USART_SDI12_RX_GL    DMA1_FLAG_GL5

#define DMA_SDI12_RX_REQUESTNUMBER    LL_DMA_REQUEST_4
#define DMA_SDI12_TX_REQUESTNUMBER    LL_DMA_REQUEST_4

#define DMA_SDI12_RX_CHNL 						LL_DMA_CHANNEL_5
#define DMA_SDI12_TX_CHNL 						LL_DMA_CHANNEL_4

#define EXTI_IRQHandler_SDI12         EXTI2_3_IRQHandler
// GPIOA.13.14 作为SWDIO，SWCLK，可以不用初始化

//******************************************
// GPIOB定义（STM32L031F6P6的GPIOB有一个IO）
//******************************************
// GPIOB.01作为传感器5V电源输出的使能端
#define SENSOR_POWER_PORT       GPIOA
#define SENSOR_POWER_PIN        LL_GPIO_PIN_5
#define SENSOR_POWER_ON()       LL_GPIO_SetOutputPin(SENSOR_POWER_PORT,SENSOR_POWER_PIN)
#define SENSOR_POWER_OFF()      LL_GPIO_ResetOutputPin(SENSOR_POWER_PORT,SENSOR_POWER_PIN)

#define M1601_DQ_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define M1601_DQ_PORT        	GPIOA
#define M1601_DQ_PIN         	LL_GPIO_PIN_15

#define M1601_DQ_OPEN_DRAIN    \
LL_GPIO_SetPinOutputType(M1601_DQ_PORT,M1601_DQ_PIN,LL_GPIO_OUTPUT_OPENDRAIN);\
LL_GPIO_SetOutputPin(M1601_DQ_PORT, M1601_DQ_PIN);

#define M1601_DQ_HIGH         LL_GPIO_SetOutputPin(M1601_DQ_PORT,M1601_DQ_PIN)
#define M1601_DQ_LOW          LL_GPIO_ResetOutputPin(M1601_DQ_PORT,M1601_DQ_PIN)
#define M1601_DQ_STATUS       LL_GPIO_IsInputPinSet(M1601_DQ_PORT,M1601_DQ_PIN)

//******************************************
// GPIOC定义（STM32L031F6P6的GPIOC有两个IO）
// GPIOC.14.15
//******************************************
// GPIOC.14.15 作为OSC32_IN,OSC32_OUT，可以不用初始化
//PC14, PC15 Special IO Init
// GPIO Function lost in STANDBY, PC14,PC15 revert to analog inputs
/*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
/* To change the source clock of the RTC feature (LSE, LSI), you have to:
- Enable the power clock
- Enable write access to configure the RTC clock source (to be done once after reset).
- Reset the Back up Domain
- Configure the needed RTC clock source */


extern void MX_GPIO_Init(void);
extern void MX_GPIO_EnterLowPower(void);
extern void MX_GPIO_ExitLowPower(void);

#endif /* GPIO_H_ */
