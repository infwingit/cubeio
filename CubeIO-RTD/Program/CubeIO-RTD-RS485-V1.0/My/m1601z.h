#ifndef __M1601_H__
#define __M1601_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "delay_utils.h"


uint8_t M1601_Init(void);
uint8_t M1601_DeInit(void);
uint8_t M1601_Measure(float* pT0);
uint8_t M1601_ResetAndPresence(void);
void M1601_SkipRom(void);
uint8_t M1601_ReadTemperature(float* pTemp);
uint8_t M1601_ReadByte(void);
void M1601_WriteByte(uint8_t data);


#ifdef __cplusplus
}
#endif

#endif /* __M1601_H__ */

