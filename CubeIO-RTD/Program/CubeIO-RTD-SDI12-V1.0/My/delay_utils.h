#ifndef DELAY_UTILS_
#define DELAY_UTILS_

#include <stdint.h>
#include "hal_timer.h"

#define Delay_Utils_Init     MX_TIM22_Delay_Init
#define Delay_Utils_DeInit   MX_TIM22_Delay_DeInit
#define Delay_Utils_DelayUs  MX_TIM22_DelayUs
#define Delay_Utils_DelayMs  MX_TIM22_DelayMs

void _delay_ms(uint16_t count);
void _delay_us(uint16_t count);

#endif /* DELAY_UTILS_ */
