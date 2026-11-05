#include "hal_eeprom.h"
#include "utils.h"

uint32_t eeprom_readint32(__IO uint32_t *Addr)
{
    uint32_t tmp=0;
    DISABLE_IRQ();
    HAL_FLASHEx_DATAEEPROM_Unlock();
    tmp=*(__IO uint32_t*)Addr;
    HAL_FLASHEx_DATAEEPROM_Lock();
    ENABLE_IRQ();
    return tmp;
}

void eeprom_writeint32(__IO uint32_t *Addr,uint32_t data)
{
    DISABLE_IRQ();
    HAL_FLASHEx_DATAEEPROM_Unlock();
    HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD,(uint32_t)Addr, data);
    HAL_FLASHEx_DATAEEPROM_Lock();
    ENABLE_IRQ();
}

float eeprom_readfloat(__IO uint32_t *Addr)
{
    UNION_LONG_VALUE ul;
    DISABLE_IRQ();
    HAL_FLASHEx_DATAEEPROM_Unlock();
    ul.uint32Value=*(__IO uint32_t*)Addr;
    HAL_FLASHEx_DATAEEPROM_Lock();
    ENABLE_IRQ();
    return ul.floatValue;
}

void eeprom_writefloat(__IO uint32_t *Addr,float data)
{
    UNION_LONG_VALUE ul;
    ul.floatValue=data;
    DISABLE_IRQ();
    HAL_FLASHEx_DATAEEPROM_Unlock();
    HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD,(uint32_t)Addr, ul.uint32Value);
    HAL_FLASHEx_DATAEEPROM_Lock();
    ENABLE_IRQ();
}
