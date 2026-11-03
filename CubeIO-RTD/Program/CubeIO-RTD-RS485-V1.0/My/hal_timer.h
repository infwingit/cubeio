#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "main.h"
#include <stdint.h>

extern void MX_TIM2_Init(void);
extern void MX_TIM2_DeInit(void);
extern void MX_TIM2_ENABLE(void);
extern void MX_TIM2_DISABLE(void);
extern void MX_TIM2_RESETCNT(void);
extern uint16_t MX_TIM2_GETCNT(void);

extern void MX_TIM21_Init(void);
extern void MX_TIM21_DeInit(void);
extern void MX_TIM21_ENABLE(void);
extern void MX_TIM21_DISABLE(void);
extern void MX_TIM21_RESETCNT(void);
extern uint16_t MX_TIM21_GETCNT(void);

extern void  MX_TIM22_Delay_Init(void);
extern void  MX_TIM22_Delay_DeInit(void);
extern void  MX_TIM22_DelayUs(uint16_t countInUs);
extern void  MX_TIM22_DelayMs(uint16_t countInMs);

extern void MySysTick_Delay_Init(void);
extern void MySysTick_DelayUs(uint16_t countInUs);
extern void MySysTick_DelayMs(uint16_t countInMs);
extern void MySysTick_Delay_DeInit(void);

#endif /* HAL_TIMER_H */
