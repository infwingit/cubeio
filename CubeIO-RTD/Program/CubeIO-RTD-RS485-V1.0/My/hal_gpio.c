#include "hal_gpio.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    //GPIO Ports Clock Enable
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    // GPIOA.2.3.4 作为RS485通信接口，本设计中不使用，设置为输出低电平
    LL_GPIO_ResetOutputPin(RS485_TXD_PORT, RS485_TXDEN_PIN);
    GPIO_InitStruct.Pin = RS485_TXDEN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(RS485_TXD_PORT, &GPIO_InitStruct);

    // GPIOA.02.03.04作为SDI12通信接口，其中SDI12_TX(PA9)复用为SDI12的中断引脚，在SDI12初始化部分进行初始化
    //根据TEROS12说明书，传感器启动后，如果是DDI输出，那么要在100ms之内输出高电平（也就意味着100ms之前可以输出低电平）
    //100毫秒之后输出的高电平表示传感器正在测量，此高电平可以用USART输出的高电平实现。
    //SDI12 TX,RX pin set as OpenDrain, 靠外部电路拉低为低电平

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_8| LL_GPIO_PIN_9 | LL_GPIO_PIN_10| LL_GPIO_PIN_11| LL_GPIO_PIN_12| LL_GPIO_PIN_15);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6| LL_GPIO_PIN_7 | LL_GPIO_PIN_10| LL_GPIO_PIN_11| LL_GPIO_PIN_12| LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // GPIOA.05作为传感器供电输出
    LL_GPIO_ResetOutputPin(SENSOR_POWER_PORT, SENSOR_POWER_PIN);
    GPIO_InitStruct.Pin = SENSOR_POWER_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SENSOR_POWER_PORT, &GPIO_InitStruct);

    //PC14, PC15 Special IO Init
    // GPIO Function lost in STANDBY, PC14,PC15 revert to analog inputs
    /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
    /* To change the source clock of the RTC feature (LSE, LSI), you have to:
       - Enable the power clock
       - Enable write access to configure the RTC clock source (to be done once after reset).
       - Reset the Back up Domain
       - Configure the needed RTC clock source */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_EnableBkUpAccess();// Enable access to LSE
    LL_RCC_LSE_Disable();// PC14 PC15 as GPIO

    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void MX_GPIO_EnterLowPower(void)
{
    //GPIO clock Enable
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    //GPIO default set to low

    //GPIO clock Disable
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOC);
}

void MX_GPIO_ExitLowPower(void)
{
    //GPIO clock Enable
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

#if 0
    //如果用到了以下引脚，则唤醒后需要重新初始化IO配置
    //PC14, PC15 need to be initialized again，
    //The PC13/PC14/PC15/PA0/PA2 GPIO functionality is lost when the core supply domain is
    //powered off (when the device enters Standby mode). In this case, if their GPIO configuration
    //is not bypassed by the RTC configuration, these pins are set in an analog input mode.
    //PC14, PC15 Special IO Init
    // GPIO Function lost in STANDBY, PC14,PC15 revert to analog inputs
    /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
    /* To change the source clock of the RTC feature (LSE, LSI), you have to:
       - Enable the power clock
       - Enable write access to configure the RTC clock source (to be done once after reset).
       - Reset the Back up Domain
       - Configure the needed RTC clock source */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_EnableBkUpAccess();// Enable access to LSE
    LL_RCC_LSE_Disable();// PC14 PC15 as GPIO

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
}
