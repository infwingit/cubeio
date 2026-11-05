#include "sensor_buffer_n.h"
#include <math.h>   /* fabsf */

/* ==========================================================================
 * N 通道 ADC 滑动窗口滤波缓冲实现
 * ========================================================================== */

/* ----------------------- 配置合法性检查（编译期） ----------------------- */
#if ADC_BUFFER_N_CHANNELS < 1
#error "ADC_BUFFER_N_CHANNELS must be >= 1"
#endif
#if ADC_BUFFER_N_CHANNELS > 16
#error "ADC_BUFFER_N_CHANNELS must be <= 16"
#endif
#if ADC_BUFFER_SIZE < 2
#error "ADC_BUFFER_SIZE must be >= 2"
#endif

/* ----------------------- 通道静态存储 ----------------------- */
/* 二维数组：第一维=通道，第二维=通道内滑动窗口数据 */
float myBufferTemp[ADC_BUFFER_N_CHANNELS][ADC_BUFFER_SIZE];
MyAdcBuffer myAdcBuffer[ADC_BUFFER_N_CHANNELS];

/* ----------------------- 内部工具 ----------------------- */
/* 返回通道在合法范围内；非法通道返回 0xFFFF，由调用方判断 */
static uint16_t channel_ok(uint16_t channel)
{
    return (channel < ADC_BUFFER_N_CHANNELS);
}

/* ----------------------- 对外接口 ----------------------- */
uint16_t adc_get_channels_n(void)
{
    return ADC_BUFFER_N_CHANNELS;
}

/* 查询某通道的平均窗口（滤波次数）
 * 温度滤波次数最多为8次，温度数值比较稳定，速度较快 */
uint16_t adc_get_filterwindow_n(uint16_t channel)
{
    (void)channel; /* 预留：若需要按通道读取 eeprom 配置，可在此展开 */
    uint16_t filterWindow = 1; // (uint16_t)eeprom_readint32(&eepUserConfig.filterWindow);
    filterWindow = filterWindow / 2;
    if(filterWindow == 0)
    {
        filterWindow = 1;
    }
    return filterWindow;
}

/* 设置某通道的平均窗口，调用方可在初始化后按需覆盖 */
void adc_set_filterwindow_n(uint16_t channel, uint16_t window)
{
    if(!channel_ok(channel))
    {
        return;
    }
    if(window == 0)
    {
        window = 1;
    }
    if(window > ADC_BUFFER_SIZE)
    {
        window = ADC_BUFFER_SIZE;   /* 窗口不得超过缓冲深度 */
    }
    myAdcBuffer[channel].ADCAvgWindowSize = window;
}

/* 初始化全部通道的环形缓冲区 */
void adc_init_buffer_n(void)
{
    for(uint16_t ch = 0; ch < ADC_BUFFER_N_CHANNELS; ch++)
    {
        MyAdcBuffer *p = &myAdcBuffer[ch];
        p->ADCBufferIndex = 0;
        p->ADCBufferCount = 0;
        p->pADCBuffer     = myBufferTemp[ch];   /* 每通道指向自己的那一段 */
        p->ADCBufferLastValue = ADC_BUFFER_INITIAL_VALUE;
        p->ADCBufferSize      = ADC_BUFFER_SIZE;
        p->ADCAvgWindowSize   = adc_get_filterwindow_n(ch);
        for(uint16_t i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            p->pADCBuffer[i] = ADC_BUFFER_INITIAL_VALUE;
        }
    }
}

/* 向某通道缓冲区添加数据（含突变检测） */
void adc_add_buffer_n(uint16_t channel, float value, float tolerance)
{
    if(!channel_ok(channel))
    {
        return;
    }
    MyAdcBuffer *p = &myAdcBuffer[channel];
    uint16_t MyBufferSize = p->ADCBufferSize;

#if 1
    /* 突变检测：本次与上次差值超过 tolerance 视为跳变，清空本通道历史填满新值 */
    float lastValue = p->ADCBufferLastValue;
    p->ADCBufferLastValue = value;
    if(fabsf(lastValue - value) > tolerance)
    {
        for(uint16_t i = 0; i < MyBufferSize; i++)
        {
            p->pADCBuffer[i] = value;
        }
    }
#endif

    /* 写入环形缓冲 */
    p->pADCBuffer[p->ADCBufferIndex] = value;
    p->ADCBufferIndex = (p->ADCBufferIndex + 1) % MyBufferSize;
    if(p->ADCBufferCount < MyBufferSize)
    {
        p->ADCBufferCount++;
    }
}

/* 计算某通道的滑动平均值（对窗口大小做防御，避免除零） */
float adc_average_buffer_n(uint16_t channel)
{
    if(!channel_ok(channel))
    {
        return 0.0f;
    }
    MyAdcBuffer *p = &myAdcBuffer[channel];
    uint16_t MyBufferSize = p->ADCBufferSize;
    uint16_t AvgWindowSize = p->ADCAvgWindowSize;

    if(AvgWindowSize == 0)
    {
        AvgWindowSize = 1;              /* 防御：窗口异常时回退为1 */
    }
    if(AvgWindowSize > p->ADCBufferCount)
    {
        AvgWindowSize = p->ADCBufferCount;   /* 数据不足时按现有数据平均 */
    }
    if(AvgWindowSize == 0)
    {
        return 0.0f;                    /* 尚无数据，返回0 */
    }

    float sum = 0;
    uint16_t currentIndex = (p->ADCBufferIndex - 1 + MyBufferSize) % MyBufferSize; /* 最新数据位置 */
    for(uint16_t i = 0; i < AvgWindowSize; i++)
    {
        sum += p->pADCBuffer[(currentIndex - i + MyBufferSize) % MyBufferSize]; /* 从最新向前取 */
    }
    return (float)(sum / AvgWindowSize);
}

/* 取某通道最新一次原始采样值 */
float adc_get_latest_n(uint16_t channel)
{
    if(!channel_ok(channel))
    {
        return 0.0f;
    }
    MyAdcBuffer *p = &myAdcBuffer[channel];
    if(p->ADCBufferCount == 0)
    {
        return 0.0f;
    }
    uint16_t idx = (p->ADCBufferIndex - 1 + p->ADCBufferSize) % p->ADCBufferSize;
    return p->pADCBuffer[idx];
}
