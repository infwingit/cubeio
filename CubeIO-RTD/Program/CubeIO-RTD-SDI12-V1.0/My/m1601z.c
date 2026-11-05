#include "m1601z.h"
#include "hal_gpio.h"
#include "utils.h"

//#define M1601_Delay_Init     Delay_Utils_Init
//#define M1601_Delay_DeInit   Delay_Utils_DeInit
//#define M1601_DelayUs        Delay_Utils_DelayUs
//#define M1601_DelayMs        Delay_Utils_DelayMs

#define M1601_DelayMs        _delay_ms

//Delay @ 4MHZ
#define M1601_Delay1Us()     \
__NOP();__NOP();__NOP();__NOP();

//Delay @ 4MHZ
#define M1601_Delay2Us()     \
M1601_Delay1Us();M1601_Delay1Us();

//Delay @ 4MHZ
#define M1601_Delay5Us() 	\
M1601_Delay1Us();M1601_Delay1Us();M1601_Delay1Us();M1601_Delay1Us();M1601_Delay1Us();

//Delay @ 4MHZ
#define M1601_Delay10Us() 	\
M1601_Delay5Us();M1601_Delay5Us();

//Delay @ 4MHZ
#define M1601_Delay15Us() 	\
M1601_Delay10Us();M1601_Delay5Us();

//Delay @ 4MHZ
#define M1601_Delay60Us() 	\
M1601_Delay15Us();M1601_Delay15Us();M1601_Delay15Us();M1601_Delay15Us();

//#define M1601Z_CONFIG_VALUE_SINGLE_HIGHREPEAT  0x02   //单次转换，Conversion Time = 10.5ms
//#define M1601Z_CONFIG_VALUE_1HZ_HIGHREPEAT     0x0A   //每秒1次，Conversion Time = 10.5ms
#define M1601Z_CONFIG_VALUE_2HZ_HIGHREPEAT     0x0E   //每秒2次，Conversion Time = 10.5ms
#define M1601Z_CONVERT_DELAY                   15     //Conversion Delay = 15ms

uint8_t M1601_Init(void)
{
    M1601_DQ_CLK_ENABLE();

    //预设为高电平输出,外部上拉电阻拉为高电平
    M1601_DQ_HIGH;
    M1601_DQ_OPEN_DRAIN;

    uint8_t error = 0;
    //M1601_Delay_Init();

    uint8_t retryCount = 3;

    while(retryCount > 0)
    {
        retryCount--;

        //M1601上电默认配置为M1601Z_CONFIG_VALUE_SINGLECONV_HIGHREPEAT，所以不用再进行设置
        //Skip ROM COMMAND and Write Config
        error = M1601_ResetAndPresence();
        if(error != 0)
        {
            continue;
        }
        M1601_SkipRom();
        M1601_WriteByte(0x4e);//Write Scratchpad Command
        M1601_WriteByte(0x00);
        M1601_WriteByte(0x00);
        M1601_WriteByte(M1601Z_CONFIG_VALUE_2HZ_HIGHREPEAT);

        //Skip ROM and Convert
        error = M1601_ResetAndPresence();
        if(error != 0)
        {
            continue;
        }
        M1601_SkipRom();
        M1601_WriteByte(0X44);
        M1601_DelayMs(2);

        //Check Conversion Done
        //主机再次释放总线，等待检测转换完成
        M1601_DQ_HIGH;
        _delay_ms(M1601Z_CONVERT_DELAY);

        //SKIP ROM and Read RAM COMMAND
        uint8_t buf[9];
        error = M1601_ResetAndPresence();
        if(error != 0)
        {
            continue;
        }
        M1601_SkipRom();
        M1601_WriteByte(0xBE);//Read Scratchpad Command
        buf[0] = M1601_ReadByte();
        buf[1] = M1601_ReadByte();
        buf[2] = M1601_ReadByte();
        buf[3] = M1601_ReadByte();
        buf[4] = M1601_ReadByte();
        buf[5] = M1601_ReadByte();
        buf[6] = M1601_ReadByte();
        buf[7] = M1601_ReadByte();
        buf[8] = M1601_ReadByte();

        if(crc8(&buf[0], 9) || buf[6] != M1601Z_CONFIG_VALUE_2HZ_HIGHREPEAT)//Check crc and config is 0x5F(0.125℃ Measurement)
        {
            error = 1;
            continue;
        }
    }

    //M1601_Delay_DeInit();
    M1601_DeInit();

    return error;
}

uint8_t M1601_DeInit(void)
{
    M1601_DQ_CLK_ENABLE();

    //预设为高电平输出,外部上拉电阻拉为高电平
    M1601_DQ_HIGH;

    //IO初始化
    M1601_DQ_OPEN_DRAIN;

    return 0;
}

uint8_t M1601_ResetAndPresence(void)
{
    //---------------------
    //主机拉低总线，至少480us
    //---------------------
    //设置为开漏输出，外部电阻拉高总线
    M1601_DQ_HIGH;
    M1601_DQ_OPEN_DRAIN;
    M1601_DelayMs(1);
    //输出低电平复位
    M1601_DQ_LOW;
    M1601_DelayMs(1);

    //---------------------
    //设置为开漏输出，外部电阻拉高总线，DS18B20等待15-60us后会给出低电平
    //---------------------
    M1601_DQ_HIGH;
    M1601_Delay15Us();

    //---------------------
    //检测DS18B20发送60-240us的低电平，表示在线，主机需检测低电平是否存在
    //---------------------
    uint8_t checkCounter = 0;
    while((M1601_DQ_STATUS == true) && checkCounter < 100)
    {
        checkCounter++;
        M1601_Delay1Us();
    }
    if(checkCounter >= 100)
    {
        return 1;
    }

    //---------------------
    //检测到DS18B20拉低总线后，主机继续监测，等待DS18B20释放总线
    //---------------------
    checkCounter = 0;
    while((M1601_DQ_STATUS == false) && checkCounter < 240)
    {
        checkCounter++;
        M1601_Delay1Us();
    }
    if(checkCounter >= 240)
    {
        return 1;
    }
    return 0;
}

uint8_t M1601_Measure(float* pTemp)
{
    uint8_t error = 0;
    //M1601_Delay_Init();

    if(pTemp != NULL)
    {
        error = M1601_ReadTemperature(pTemp);
    }

    //M1601_Delay_DeInit();
    M1601_DeInit();

    return error;
}

void M1601_WriteByte(uint8_t data)
{
    uint8_t i = 0;

    M1601_DQ_HIGH;

    DISABLE_IRQ();
    for(i = 0; i < 8; i++)
    {
        M1601_Delay2Us();

        //主机持续拉低5us，然后输出0或者1，表示写0或者写1，应在主机开始拉低时15us内完成写0或者写1的设置
        //因为M1601Z会在主机开始拉低时的弟15-60us内读取数据
        M1601_DQ_LOW;
        M1601_Delay5Us();

        //主机输出数据后，DS18B20在45us内采样
        if(data & 0x01)
        {
            M1601_DQ_HIGH;
        }
        data >>= 1;
        M1601_Delay60Us();

        //主机恢复高电平
        M1601_DQ_HIGH;
    }
    ENABLE_IRQ();
}

unsigned char M1601_ReadByte(void)
{
    uint8_t i = 0, data = 0;

    M1601_DQ_HIGH;

    DISABLE_IRQ();
    for(i = 0; i < 8; i++)
    {
        //主机拉低至少5us后释放总线，M1601在主机开始拉低的15us内进行数据输出
        M1601_DQ_LOW;
        M1601_Delay5Us();
        M1601_DQ_HIGH;
        data >>= 1;
        M1601_Delay5Us();

        if(M1601_DQ_STATUS == true)
        {
            data |= 0x80;
        }
        //M1601在主机开始拉低的45us内恢复响应
        M1601_Delay60Us();

        //主机再次释放总线
        M1601_DQ_HIGH;
        M1601_Delay2Us();
    }
    ENABLE_IRQ();

    return data;
}

void M1601_SkipRom(void)
{
    M1601_WriteByte(0XCC);
}

uint8_t M1601_ReadTemperature(float* pTemp)
{
    uint8_t error = 0;
    uint8_t retryCount = 3;

    while(retryCount > 0)
    {
        retryCount--;

        //SKIP ROM and Read RAM COMMAND
        uint8_t buf[9];
        error = M1601_ResetAndPresence();
        if(error != 0)
        {
            continue;
        }
        M1601_SkipRom();
        M1601_WriteByte(0xBE);//Read Scratchpad Command
        buf[0] = M1601_ReadByte();
        buf[1] = M1601_ReadByte();
        buf[2] = M1601_ReadByte();
        buf[3] = M1601_ReadByte();
        buf[4] = M1601_ReadByte();
        buf[5] = M1601_ReadByte();
        buf[6] = M1601_ReadByte();
        buf[7] = M1601_ReadByte();
        buf[8] = M1601_ReadByte();

        if(crc8(&buf[0], 9) || buf[6] != M1601Z_CONFIG_VALUE_2HZ_HIGHREPEAT)//Check crc and config
        {
            continue;
        }
        else
        {
            int16_t temp;
            temp = buf[1];
            temp <<= 8;
            temp |= buf[0];
            *pTemp = 40.00 + temp / 256.00;
            break;
        }
    }

    if(retryCount == 0)
    {
        return 1;
    }
    return 0;
}
