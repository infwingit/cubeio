#include "delay_utils.h"
#include "main.h"

#if 0
//Delay @ 16MHZ
#define NOP_1US() 	\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

//Delay @ 16MHZ
#define NOP_10US() 	\
NOP_1US();NOP_1US();NOP_1US();NOP_1US();NOP_1US();\
NOP_1US();NOP_1US();NOP_1US();NOP_1US();NOP_1US();

//Delay @ 2MHZ
#define NOP_1US_2MHZ() 	\
__NOP();__NOP();

//Delay @ 2MHZ
#define NOP_10US_2MHZ() 	\
NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();\
NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();NOP_1US_2MHZ();

#endif


//Delay @ 4MHZ
#define NOP_1US_4MHZ() 	\
__NOP();__NOP();__NOP();__NOP();

//Delay @ 4MHZ
#define NOP_10US_4MHZ() 	\
NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();\
NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();NOP_1US_4MHZ();

void _delay_ms(uint16_t count)
{
    uint32_t i;
    uint32_t ctr = count * 10;
    for(i = 0; i < ctr; i++)
    {
        //0.1ms
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
        NOP_10US_4MHZ();
    }
}

void _delay_us(uint16_t count)
{
    uint16_t i;
    for(i = 0; i < count; i++)
    {
        NOP_1US_4MHZ();
    }
}
