#include "delay_utils.h"
#include "main.h"

//Delay @ 16MHZ
#define NOP_1US() 	\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

//Delay @ 16MHZ
#define NOP_10US() 	\
NOP_1US();NOP_1US();NOP_1US();NOP_1US();NOP_1US();\
NOP_1US();NOP_1US();NOP_1US();NOP_1US();NOP_1US();

void _delay_ms(uint16_t count)
{
    uint32_t i;
    uint32_t ctr = count * 10;
    for(i = 0; i < ctr; i++)
    {
        //0.1ms
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
        NOP_10US();
    }
}

void _delay_us(uint16_t count)
{
    uint16_t i;
    for(i = 0; i < count; i++)
    {
        NOP_1US();
    }
}
