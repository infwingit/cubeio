#ifndef HAL_EEPROM_H_
#define HAL_EEPROM_H_

#include "main.h"

//******************************************
// EEPROM Read Write
//******************************************
extern uint32_t eeprom_readint32(__IO uint32_t *Addr);
extern void eeprom_writeint32(__IO uint32_t *Addr,uint32_t data);
extern float eeprom_readfloat(__IO uint32_t *Addr);
extern void eeprom_writefloat(__IO uint32_t *Addr,float data);

#endif
