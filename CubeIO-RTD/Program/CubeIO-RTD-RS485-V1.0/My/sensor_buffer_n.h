#ifndef SENSOR_BUFFER_N_H_
#define SENSOR_BUFFER_N_H_

#include "utils.h"
#include "eeprom.h"
#include <stdint.h>

/* ==========================================================================
 * N 通道 ADC 滑动窗口滤波缓冲模块（多通道版）
 * 通道数 N 由宏 ADC_BUFFER_N_CHANNELS 配置，取值范围 1 ~ 16。
 * 逻辑与单通道版一致：环形滑动窗口平均 + 突变检测（跳变>tolerance则
 * 清空本通道历史、填满新值，使均值立刻跟随新台阶）。
 * ========================================================================== */

/* ----------------------- 可配置宏 ----------------------- */
#ifndef ADC_BUFFER_N_CHANNELS
#define ADC_BUFFER_N_CHANNELS       16      /* 通道数 N，1 ~ 16 */
#endif

#ifndef ADC_BUFFER_SIZE
#define ADC_BUFFER_SIZE             17     /* 每通道缓冲深度（滑动窗口容量） */
#endif

#ifndef ADC_BUFFER_INITIAL_VALUE
#define ADC_BUFFER_INITIAL_VALUE   (-32768.0f)  /* 无效/初始值 */
#endif

/* ----------------------- 数据结构 ----------------------- */
typedef struct MyAdcBuffer
{
    float    *pADCBuffer;            /* 指向本通道的数据缓冲 */
    float     ADCBufferLastValue;    /* 上次采样值，突变检测用（float，避免截断） */
    uint16_t  ADCBufferSize;         /* 本通道缓冲大小 */
    uint16_t  ADCBufferIndex;        /* 本通道当前写入位置 */
    uint16_t  ADCBufferCount;        /* 本通道当前已存数据数量 */
    uint16_t  ADCAvgWindowSize;      /* 本通道平均窗口大小 */
} MyAdcBuffer;

/* ----------------------- 接口 ----------------------- */
uint16_t adc_get_channels_n(void);                                   /* 返回实际通道数 N */
uint16_t adc_get_filterwindow_n(uint16_t channel);                   /* 查询某通道平均窗口 */
void     adc_set_filterwindow_n(uint16_t channel, uint16_t window);  /* 设置某通道平均窗口 */
void     adc_init_buffer_n(void);                                    /* 初始化全部通道 */
void     adc_add_buffer_n(uint16_t channel, float value, float tolerance); /* 向某通道写入采样 */
float    adc_average_buffer_n(uint16_t channel);                     /* 求某通道滑动平均 */
float    adc_get_latest_n(uint16_t channel);                         /* 取某通道最新原始采样 */

#endif /* SENSOR_BUFFER_N_H_ */
