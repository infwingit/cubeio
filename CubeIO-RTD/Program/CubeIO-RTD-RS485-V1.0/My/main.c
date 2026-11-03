#include "main.h"
#include "globals.h"
#include "eeprom.h"
#include "utils.h"
#include <string.h>
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_eeprom.h"
#include "delay_utils.h"
#include "modbus.h"
#include "sensor.h"
#include "utils_datetime.h"
#include "sensor_buffer_n.h"
#include "m1601z.h"
//WWDG_HandleTypeDef hwwdg;

void SystemTick_Init(void);
void SystemTick_DeInit(void);
void SystemClock_Config(void);
void SystemClock_SetAs16MHZ(void);
void SystemClock_SetAs24MHZ(void);

static void MX_NVIC_Init(void);
static void MX_IWDG_Init(void);
//static void MX_WWDG_Init(void);

static UTIL_TIMER_Object_t TimerWatchdog;
static UTIL_TIMER_Object_t TimerAcqInterval;
static UTIL_TIMER_Object_t TimerAcq;
UTIL_TIMER_Object_t TimerSystemReset;//通信程序中使用，不能设置为static变量

static void OnTimerWatchdog(void* context)
{
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_WatchDog), 1);
}

static void OnTimerAcqInterval(void* context)
{
    //采集周期
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_AcqInterval), 1);
}

static void OnTimerAcq(void* context)
{
    //分时采集
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Acq), 1);
}

static void OnTimerSystemReset(void* context)
{
    NVIC_SystemReset();
}

static void TaskRS485Process(void)
{
    if(!modbusRtuTransValidate(&g_usartInfo))   //检查接受到的数据是否正确
    {
        modbusRtuRequest2Respond(&g_usartInfo);
        MX_RS485_Transmit_DMA(&g_usartInfo);
    }
    else
    {
        MX_RS485_Receive_DMA(&g_usartInfo);
    }
}

static void TaskAcqIntervalProcess(void)
{
    //初始化测量步骤，从上电开始
    g_MValue.SensorMeasureStep = SensorMeasureStepInit;

    //设置测量状态标志
    g_MValue.SensorMeasureStatus = SensorMeasureStatusStart;

    //启动定时器，开始进行测量
    UTIL_TIMER_Start(&TimerAcq);
}

static void TaskAcqProcess(void)
{
    int16_t error = 0;

    EnumSensorMeasureStep sensorMeasureStepDone;
    error += SensorMeasureAsync(&sensorMeasureStepDone);
}

static void TaskWatchDogProcess(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}

//#define DEBUG 1  //条件编译选项中也可设置
int main(void)
{
#if DEBUG
    //STOP模式下允许调试
    __HAL_RCC_DBGMCU_CLK_ENABLE();
    HAL_DBGMCU_EnableDBGStopMode();
    //调试时禁用以下外设
    LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_IWDG_STOP);
    LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_RTC_STOP);
    LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);
    LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM21_STOP);
    LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM22_STOP);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
#endif

    //配置系统时钟，这里配置了HSI和PLL，并使用HSI16作为默认的上电时钟
    SystemClock_Config();
    //SystemClock_SetAs16MHZ();
    //SysTick配置，SYSTICK在STOP模式时自动停止，唤醒后自动启动（参见stm32l031x46 Hardware manual.pdf， Table 5. Functionalities depending on the working mode (from Run/active down to standby)）
    //根据《dui0662a_cortex_m0p_r0p0_dgug.pdf》，P102， The interrupt controller clock updates the SysTick counter. If this clock signal is stopped for low power mode, the SysTick counter stops.
    //SYSTICK是CORTEX的一个向下计数器，下降到0时中断，系统中设置1ms中断，在SysTick_Handler中断中使用HAL_IncTick()对uwTick++
    //SYSTICK在STOP模式下是自动停止的，但是在DEBUG使能的情况下，当连接仿真器时，即使进入了STOP模式，由于CORE时钟支持DEBUG没有被禁用，所以SYSTICK还是运行的
    SystemTick_Init();

    _delay_ms(100);//上电等待电源稳定

    //以下两个设置本应在HAL_MspInit中完成，现在放在这里
    //ReferenceManual P197-System configuration controller clock enable bit(System configuration controller clock enabled)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    //ReferenceManual P198-Power interface clock enable bit(Power interface clock enabled)
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    //看门狗使能在先，应在LSE初始化前，初始化看门狗
    MX_IWDG_Init();
    //MX_WWDG_Init();

    globalVarInit();
    eeprom_init();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    M1601_Init();
    SensorDeInit();//进入低功耗
    MX_NVIC_Init();

#if 0
    while(1)
    {
        RS485_RXDEN_OFF();
        _delay_ms(50);
        RS485_RXDEN_ON();
        _delay_ms(50);
    }

    MX_TIM22_Delay_Init();
    while(1)
    {
        MX_TIM22_DelayUs(5000);
        AS3935_TRIG_ON();
        MX_TIM22_DelayUs(5000);
        AS3935_TRIG_OFF();
    }
    MX_TIM22_Delay_DeInit();

    while(1)
    {
        AS3935_TRIG_ON();
        HAL_Delay(10);
        AS3935_TRIG_OFF();
        HAL_Delay(10);
    }
#endif

    UTIL_SEQ_Init();
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_RS485), UTIL_SEQ_RFU, TaskRS485Process);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_AcqInterval), UTIL_SEQ_RFU, TaskAcqIntervalProcess);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Acq), UTIL_SEQ_RFU, TaskAcqProcess);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_WatchDog), UTIL_SEQ_RFU, TaskWatchDogProcess);

    /*Initialize timer and RTC*/
    UTIL_TIMER_Init();
    SystemTick_DeInit();
    UTIL_TIMER_Create(&TimerWatchdog, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnTimerWatchdog, NULL);
    UTIL_TIMER_SetPeriod(&TimerWatchdog, 10000);//看门狗时间28秒，这里10秒复位一次，确保有任务执行的情况下不至于让看门狗自动复位
    UTIL_TIMER_Start(&TimerWatchdog);
    //UTIL_TIMER_Stop(&TimerWatchdog);

    UTIL_TIMER_Create(&TimerAcqInterval, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnTimerAcqInterval, NULL);

    UTIL_TIMER_Create(&TimerAcq, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnTimerAcq, NULL);
    UTIL_TIMER_SetPeriod(&TimerAcq, 10);
		UTIL_TIMER_Start(&TimerAcq);

    UTIL_TIMER_Create(&TimerSystemReset, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerSystemReset, NULL);
    UTIL_TIMER_SetPeriod(&TimerSystemReset, 1000);

    MX_RS485_Init();

    //Init low power manager
    UTIL_LPM_Init();
    UTIL_LPM_SetOffMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);//Disable Stand-by mode
    //UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);//Disable Stop Mode
    UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);//Disable Stop Mode

    while(1)
    {
        UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
    }
}

//OverWrite the HAL_InitTick function
//1ms tick
void SystemTick_Init(void)
{
    HAL_SYSTICK_Config(SystemCoreClock / 1000UL);
}

void SystemTick_DeInit(void)
{
    // 1. 关闭 NVIC 中断通道 (核心：防止响应中断)
    HAL_NVIC_DisableIRQ(SysTick_IRQn);

    // 2. 清除挂起中断标志 (你说的这个！正确！)
    HAL_NVIC_ClearPendingIRQ(SysTick_IRQn);

    // 3. 关闭 SysTick 计数器、时钟、中断源
    SysTick->CTRL = 0;

    // 4. 清空计数器值 (干净彻底)
    SysTick->VAL = 0;
}

void SystemClock_SetAs16MHZ()
{
    //要先使能HSI并切换到HSI，才能关闭其他比如PLL作为系统时钟, 否则系统就没有时钟了，会出现异常情况
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
    {
    }

    LL_SetSystemCoreClock(16000000);
    LL_RCC_PLL_Disable();//PLL禁止后并不会清空PLL的分频系数等RCC寄存器的数据
}

void SystemClock_SetAs24MHZ()
{
    //PLL使用了HSI所以此处不能禁止HSI
    LL_RCC_PLL_Enable();
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    }

    LL_SetSystemCoreClock(24000000);
}

//STM32L031时钟结构：HSI(LL_RCC_HSI_EnableDivider) 16MHZ/4MHZ--> AHB(LL_RCC_SetAHBPrescaler) -> APB1/APB2(LL_RCC_SetAPB1Prescaler,LL_RCC_SetAPB2Prescaler)
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
    {
        Error_Handler();
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);//AlexReviewedAndUpdated

    //HSI配置，并等待准备好
    LL_RCC_HSI_EnableDivider();//16MHZ/4 = 4MHZ
    LL_RCC_HSI_Enable();

    while(LL_RCC_HSI_IsReady() != 1)
    {
    }

    LL_RCC_HSI_SetCalibTrimming(16);

    //上电默认使用HSI时钟
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    //LL_RCC_HSI_EnableInStopMode();//HSI在STOP时继续工作， 这里需要禁用!!!!!!
    LL_SetSystemCoreClock(4000000);

    //PLL配置,这里主要设置分频系数，关闭PLL后这些系数是保存的，以备中断后切换时钟为PLL使用
    //不使能PLL的情况下，也是可以设置分频系数的
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_4);

    //设置系统分频
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);//4MHz/1=4MHZ(CPU运行频率)
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);//4MHz/1
    //AlexReviewedAndUpdated-APB2 connect to TIM21,TIM22 and ADC
    //对于TIM21,TIM22，如果APB2Prescaler=1那么TIM21,TIM22的输入时钟频率X1，如果APB2Prescaler=2,4,8那么TIM21,TIM 22的输入时钟频率X2
    //所以对于以下设置之后，TIM21，TIM22的时钟频率还是4MHZ
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);//4MHz/1

    //只有在切换为其他系统时钟后，才能禁用MSI，因为上电MSI为默认时钟
    LL_RCC_MSI_Disable();
    //LL_RCC_LSI_Disable();//这里不再禁用LSI，因为IWDG和RTC使用LSI作为时钟源
}

static void MX_NVIC_Init(void)
{
    //NVIC是CORTEX的中断向量管理，EXTI是STM32的中断管理
    //EXTI配置后最终连接到NVIC（EXTI Line type = Configurable）
    //也有STM32的外设中断直接连到NVIC的（EXTI Line type = Direct）

    //STM32L0的中断优先级数值越小，优先级越高

    /*-2- Connect External Line to the GPIO*/
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    //STM32L0 User Manual, SYSCFG chapter, SYSCFG_EXTICR
    //These bits are written by software to select the source input for the EXTIx external interrupt.
    //https://www.cnblogs.com/anSn/p/11633462.html
    //LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE6);

    /*-3- Enable a falling trigger EXTI line 1 Interrupt */
    /* Set fields of initialization structure */
    /* Init with LL driver */
    //LL_EXTI_InitTypeDef exti_initstruct;
    //exti_initstruct.Line_0_31   = CS1237_DATA_AS_EXTI_LINE;
    //exti_initstruct.LineCommand = ENABLE;
    //exti_initstruct.Mode        = LL_EXTI_MODE_IT;
    //exti_initstruct.Trigger     = LL_EXTI_TRIGGER_FALLING;
    //LL_EXTI_Init(&exti_initstruct);

    //配置GPIO中断优先级，使能NVIC中的相应中断
    //NVIC_SetPriority(CS1237_DATA_AS_EXTI_IRQn, 0);//The lower value, the higher priority
    //NVIC_EnableIRQ(CS1237_DATA_AS_EXTI_IRQn);

    //UART中断优先级（EXTI Line type = Direct）
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);

    //TIM21中断优先级（EXTI Line type = Direct）
    NVIC_SetPriority(TIM21_IRQn, 2);
    NVIC_EnableIRQ(TIM21_IRQn);

    //LPUART中断优先级（EXTI Line type = Direct）
    //NVIC_SetPriority(LPUART1_IRQn, 0);
    //NVIC_EnableIRQ(LPUART1_IRQn);

    /* (1) Enable the clock of DMA1 */
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    //NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0);
    //NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
}

static void MX_IWDG_Init(void)
{
//  LL_IWDG_Enable(IWDG);
//  LL_IWDG_EnableWriteAccess(IWDG);
//  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);
//  LL_IWDG_SetReloadCounter(IWDG, 4095);
//  while (LL_IWDG_IsReady(IWDG) != 1)
//  {
//  }
//  LL_IWDG_SetWindow(IWDG, 4095);
//  LL_IWDG_ReloadCounter(IWDG);


    /* Enable the peripheral clock IWDG */
    /* -------------------------------- */
    LL_RCC_LSI_Enable();
    while(LL_RCC_LSI_IsReady() != 1)
    {
    }

    /* Configure the IWDG with window option disabled */
    /* ------------------------------------------------------- */
    /* (1) Enable the IWDG by writing 0x0000 CCCC in the IWDG_KR register */
    /* (2) Enable register access by writing 0x0000 5555 in the IWDG_KR register */
    /* (3) Write the IWDG prescaler by programming IWDG_PR from 0 to 7 - LL_IWDG_PRESCALER_4 (0) is lowest divider*/
    /* (4) Write the reload register (IWDG_RLR) */
    /* (5) Wait for the registers to be updated (IWDG_SR = 0x0000 0000) */
    /* (6) Refresh the counter value with IWDG_RLR (IWDG_KR = 0x0000 AAAA) */
    // 37KHZ/LL_IWDG_PRESCALER_256= 144.5HZ(0.00692s=6.92ms), 6.92ms*4046(0x0FCE) = 28.00s
    LL_IWDG_Enable(IWDG);                             /* (1) */
    LL_IWDG_EnableWriteAccess(IWDG);                  /* (2) */
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_256);  /* (3) */
    LL_IWDG_SetReloadCounter(IWDG, 0x0FCE);            /* (4) */
    while(LL_IWDG_IsReady(IWDG) != 1)                 /* (5) */
    {
    }
    LL_IWDG_ReloadCounter(IWDG);                      /* (6) 复位看门狗*/
}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
//static void MX_WWDG_Init(void)
//{

//  /* USER CODE BEGIN WWDG_Init 0 */

//  /* USER CODE END WWDG_Init 0 */

//  /* USER CODE BEGIN WWDG_Init 1 */

//  /* USER CODE END WWDG_Init 1 */
//  hwwdg.Instance = WWDG;
//  hwwdg.Init.Prescaler = WWDG_PRESCALER_1;
//  hwwdg.Init.Window = 64;
//  hwwdg.Init.Counter = 64;
//  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
//  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN WWDG_Init 2 */

//  /* USER CODE END WWDG_Init 2 */

//}

void  MX_TIM21_Init(void)
{
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    //Peripheral clock enable
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM21);

    //Init timebase
    //AlexReviewedAndUpdated-APB2 connect to TIM21,TIM22 and ADC
    //对于TIM21,TIM22，如果APB2Prescaler=1那么TIM21,TIM22的输入时钟频率X1，如果APB2Prescaler=2,4,8那么TIM21,TIM 22的输入时钟频率X2
    //所以对于以下设置之后，TIM21，TIM22的时钟频率还是16MHZ
    TIM_InitStruct.Prescaler = 1600;//timebase=0.1ms, 1600 for 16MHZ
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 10 - 1; //1ms interrupt
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM21, &TIM_InitStruct);

    //Auto Preload
    LL_TIM_EnableARRPreload(TIM21);

    //Clock Source
    LL_TIM_SetClockSource(TIM21, LL_TIM_CLOCKSOURCE_INTERNAL);

    //SlaveMode disabled
    LL_TIM_DisableMasterSlaveMode(TIM21);

    //OnePulse Mode (Use LL_TIM_EnableCounter to reset value and start timer )
    //LL_TIM_SetOnePulseMode(TIM21, LL_TIM_ONEPULSEMODE_SINGLE);

    //Enable Interrupt
    LL_TIM_EnableIT_UPDATE(TIM21);

    //Start Timer
    LL_TIM_EnableCounter(TIM21);
}

void Error_Handler(void)
{
    //如果出现严重错误则重新启动
    while(1)
    {
        NVIC_SystemReset();
    }
}
