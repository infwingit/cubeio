#include "hal_timer.h"
#include "main.h"

void  MX_TIM2_Init(void)
{
    //为了加快执行速度，我们不使用LL函数进行设置，直接设置寄存器
    //注意这是针对16MHZ的时钟频率设置的
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    //Peripheral clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    //Init timebase
    TIM_InitStruct.Prescaler = 16000;//timebase=16000000/16000=1000HZ(1ms)
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 60000;//最长超时时间60000ms
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    //Auto Preload
    LL_TIM_EnableARRPreload(TIM2);

    //Clock Source
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    //SlaveMode disabled
    LL_TIM_DisableMasterSlaveMode(TIM2);

    //OnePulse Mode (Use LL_TIM_EnableCounter to reset value and start timer )
    //LL_TIM_SetOnePulseMode(TIM2, LL_TIM_ONEPULSEMODE_SINGLE);

    //Start Timer
    LL_TIM_EnableCounter(TIM2);
}

void  MX_TIM2_DeInit(void)
{
    //要先禁用TIMER，然后再禁用时钟，否则禁用不掉TIMER
    LL_TIM_DisableCounter(TIM2);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
}

__INLINE  void MX_TIM2_ENABLE(void)
{
    LL_TIM_EnableCounter(TIM2);
}

__INLINE  void MX_TIM2_DISABLE(void)
{
    LL_TIM_DisableCounter(TIM2);
}

__INLINE  void MX_TIM2_RESETCNT(void)
{
    WRITE_REG(TIM2->CNT, 0);
}

__INLINE  uint16_t MX_TIM2_GETCNT(void)
{
    return (uint16_t)READ_REG(TIM2->CNT);
}

//TIM21 is used for Period Interrupt for SlabSense-RS485
#if 0
void  MX_TIM21_Init(void)
{
    //为了加快执行速度，我们不使用LL函数进行设置，直接设置寄存器
    //注意这是针对24MHZ的时钟频率设置的
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    //Peripheral clock enable
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM21);

    //Init timebase
    TIM_InitStruct.Prescaler = 16000;//timebase=16000000/16000=1000HZ(1ms)
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 60000;//最长超时时间60000ms
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

    //Start Timer
    LL_TIM_EnableCounter(TIM21);
}

void  MX_TIM21_DeInit(void)
{
    //要先禁用TIMER，然后再禁用时钟，否则禁用不掉TIMER
    LL_TIM_DisableCounter(TIM21);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM21);
}

__INLINE  void MX_TIM21_ENABLE(void)
{
    LL_TIM_EnableCounter(TIM21);
}

__INLINE  void MX_TIM21_DISABLE(void)
{
    LL_TIM_DisableCounter(TIM21);
}

__INLINE  void MX_TIM21_RESETCNT(void)
{
    WRITE_REG(TIM21->CNT, 0);
}

__INLINE  uint16_t MX_TIM21_GETCNT(void)
{
    return (uint16_t)READ_REG(TIM21->CNT);
}
#endif

void  MX_TIM22_Delay_Init(void)
{
    /* Enable the timer peripheral clock */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM22);

    /* Set counter mode */
    /* Reset value is LL_TIM_COUNTERMODE_UP */
    LL_TIM_SetCounterMode(TIM22, LL_TIM_COUNTERMODE_UP);

    /* Set the pre-scaler value to have TIM counter clock equal to 1 MHz      */
    //AlexReviewedAndUpdated-APB2 connect to TIM21,TIM22 and ADC
    //对于TIM21,TIM22，如果APB2Prescaler=1那么TIM21,TIM22的输入时钟频率X1，如果APB2Prescaler=2,4,8那么TIM21,TIM 22的输入时钟频率X2
    //所以对于以下设置之后，TIM21，TIM22的时钟频率还是16MHZ
    LL_TIM_SetPrescaler(TIM22, 16);//16MHZ/16=1MHZ

    /* Set the auto-reload value to have an initial update event frequency of 10 Hz */
    //uint16_t InitialAutoreload = __LL_TIM_CALC_ARR(SystemCoreClock, LL_TIM_GetPrescaler(TIM22), 10);
    LL_TIM_SetAutoReload(TIM22, 65535);//最长时间65535*1us=65535us

    /* Enable counter */
    LL_TIM_EnableCounter(TIM22);

    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIM22);
}

void  MX_TIM22_Delay_DeInit(void)
{
    // Disable Counter
    LL_TIM_DisableCounter(TIM22);

    /* Enable the timer peripheral clock */
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM22);
}

//参数countInUs应小于65535
void  MX_TIM22_DelayUs(uint16_t countInUs)
{
    LL_TIM_SetCounter(TIM22, 0);
    while(LL_TIM_GetCounter(TIM22) < countInUs)
    {
    }
}

//参数countInMs应小于65535
void  MX_TIM22_DelayMs(uint16_t countInMs)
{
    for(uint16_t i = 0; i < countInMs; i++)
    {
        MX_TIM22_DelayUs(1000);
    }
}

#if 1
/**
  \ AlexReviewedAndUpdated - 使用系统提供的SysTick_Config函数进行延时操作
  \__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
  \brief   System Tick Configuration
  \details Initializes the System Timer and its interrupt, and starts the System Tick Timer.
           Counter is in free running mode to generate periodic interrupts.
  \param [in]  ticks  Number of ticks between two interrupts.
  \return          0  Function succeeded.
  \return          1  Function failed.
  \note    When the variable <b>__Vendor_SysTickConfig</b> is set to 1, then the
           function <b>SysTick_Config</b> is not included. In this case, the file <b><i>device</i>.h</b>
           must contain a vendor-specific implementation of this function.
 */
//Stop 模式SysTick 时钟会随内核时钟停止，无额外功耗；Sleep 模式若 SysTick 中断使能，会周期性唤醒内核，直接破坏低功耗效果，导致功耗大幅上升
void MySysTick_Delay_Init(void)
{
		//这里只开启SysTick，但不使能中断
    SysTick->LOAD  = (uint32_t)(SysTick_LOAD_RELOAD_Msk);        /* set reload register */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}

void MySysTick_DelayUs(uint16_t countInUs)
{
    uint32_t load = SystemCoreClock / countInUs;

    SysTick->LOAD  = (uint32_t)(load - 1UL);                         /* set reload register */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}

void MySysTick_DelayMs(uint16_t countInMs)
{
    for(uint16_t i = 0; i < countInMs; i++)
    {
        MySysTick_DelayUs(1000);
    }
}

void MySysTick_Delay_DeInit(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
#endif
