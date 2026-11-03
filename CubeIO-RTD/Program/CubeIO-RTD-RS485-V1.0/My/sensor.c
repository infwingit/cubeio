#include <math.h>
#include "sensor.h"
#include "sensor_buffer_n.h"
#include "globals.h"
#include "eeprom.h"
#include "hal_gpio.h"
#include "utils.h"
#include "hal_eeprom.h"
#include "delay_utils.h"
#include "rtc_if.h"
#include "hal_spi.h"
#include "ads1220.h"
#include "m1601z.h"

MEASURE_VALUE g_MValue;

int16_t SensorInitPowerOn(void)
{
    int16_t errorCode = 0;
    errorCode += SensorInit();
    return errorCode;
}

int16_t SensorInit(void)
{
    int16_t errorCode = 0;
    adc_init_buffer_n();
    SENSOR_POWER_ON();
    _delay_ms(10);//Wait power supply stable
    SPI_Soft_Init();
    errorCode += ADS1220_Init(ADS1220_GAIN_32, ADS1220_DR_NORMAL_20);
    return errorCode;
}

int16_t SensorDeInit(void)
{
    int16_t errorCode = 0;
    errorCode = ADS1220_Deinit();

    //20260217经测试，在接上负载板（模拟输入板）后，100ms可使电源电压下降到1V以下。
    SENSOR_POWER_OFF();
    _delay_ms(20);

    return errorCode;
}

#define ADS1220_CONV_TIMEOUT_IN_10MS  10
int16_t SensorMeasureAsync(EnumSensorMeasureStep* pSensorMeasureStepDone)
{
    int16_t errorCode = 0;
    *pSensorMeasureStepDone = g_MValue.SensorMeasureStep;
    switch(g_MValue.SensorMeasureStep)
    {
    case SensorMeasureStepDeInit:
    {
        g_MValue.SensorDeInitCounter++;
        if(g_MValue.SensorDeInitCounter > 3)
        {
            //到达计数次数后，执行下一步，进行上电初始化
            g_MValue.SensorDeInitCounter = 0;
            g_MValue.SensorMeasureStep = SensorMeasureStepInit;
        }
        else
        {
            //没有到达计数次数，继续进行反向初始化，保证电容掉电
            errorCode += SensorDeInit();
            g_MValue.SensorMeasureStep = SensorMeasureStepDeInit;
        }
        break;
    }
    case SensorMeasureStepInit:
    {
        errorCode += SensorInitPowerOn();
        g_MValue.SensorMeasureStep = SensorMeasureStepTempStartADS1220Async;
        break;
    }
    case SensorMeasureStepTempStartM1601Async:
    {
        g_MValue.measTimeStart = RTC_IF_GetTimerValue();

        g_MValue.TempStatusM1601 = SensorMeasureTempStartM1601Async();
        g_MValue.SensorMeasureStep = SensorMeasureStepTempStartADS1220Async;

        g_MValue.measTimeTempStartM1601 = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepTempStartADS1220Async:
    {
        errorCode += SensorMeasureTempStartADS1220Async();//这里会检验ADS1220的通信是否正常，以在外部设置传感器状态
        g_MValue.SensorMeasureStep = SensorMeasureStepTempReadADS1220Async;
        g_MValue.ADS1220ConvTimeoutIn10ms = ADS1220_CONV_TIMEOUT_IN_10MS;
        g_MValue.measTimeTempStartADS1220 = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepTempReadADS1220Async:
    {
        uint8_t isConversionDone = 1;
        errorCode += SensorMeasureTempReadADS1220Async(&isConversionDone, &g_MValue.ADS1220TempC);
        if(isConversionDone)
        {
            g_MValue.SensorMeasureStep = SensorMeasureStepTempCalcAmbientAsync;
            g_MValue.TempStatusADS1220 = 0;
            ADS1220_SetReg1EnableTemperatureSensor(ADS1220_TEMPSENSOR_DISABLE);
            g_MValue.measTimeTempReadADS1220 = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        }
        else
        {
            g_MValue.SensorMeasureStep = SensorMeasureStepTempReadADS1220Async;
            _delay_ms(10);
            if((g_MValue.ADS1220ConvTimeoutIn10ms--) == 0)
            {
                //ADC转换超时，在外部处理
                g_MValue.TempStatusADS1220 = 1;
                errorCode = 1;
            }
        }
        break;
    }
    case SensorMeasureStepTempCalcAmbientAsync:
    {
        errorCode += SensorMeasureTempCalcAmbientAsync();
        g_MValue.SensorMeasureStep = SensorMeasureStepChannelPrepareConv;
        g_MValue.measTimeTempCalcAmbient = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepChannelPrepareConv:
    {
        g_MValue.SensorChnlIndexCurrent = SENSOR_CHANNEL_INDEX_0;//从第0通道开始（新一轮）转换
        g_MValue.SensorMeasureStep = SensorMeasureStepChannelInitConv;
        g_MValue.measTimeChannelPrepareConv = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepChannelInitConv:
    {
        //判断当前通道是否被禁用
        uint32_t  ChnlEnable = (uint32_t) eeprom_readint32((uint32_t*) & (eepChannelConfig.ItemConfigs[g_MValue.SensorChnlIndexCurrent].ChnlEnable));
        if(ChnlEnable == CONST_CHNL_ENABLE_DISABLE)
        {
            //如果被禁用则不进行转换，直接跳转到下一通道
        }
        else
        {
            //当前通道需要进行转换
            g_MValue.ChnlFilterOrder = (uint32_t) eeprom_readint32((uint32_t*) & (eepChannelConfig.ItemConfigs[g_MValue.SensorChnlIndexCurrent].FilterOrder));
            g_MValue.ChnlFilterOrderCounter = 0;

            errorCode += SensorMeasureChannelInitAsync(g_MValue.SensorChnlIndexCurrent);
            g_MValue.SensorMeasureStep = SensorMeasureStepChannelWaitStableAsync;
        }
        g_MValue.measTimeChannelInitConv = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepChannelWaitStableAsync:
    {
        _delay_ms(10);//延时等待MUX与模拟量输入稳定
        g_MValue.SensorMeasureStep = SensorMeasureStepChannelStartConvAsync;
        g_MValue.measTimeChannelWaitStable = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepChannelStartConvAsync:
    {
        ADS1220_Start();
        g_MValue.ChnlFilterOrderCounter++;
        g_MValue.ADS1220ConvTimeoutIn10ms = ADS1220_CONV_TIMEOUT_IN_10MS;
        g_MValue.SensorMeasureStep = SensorMeasureStepChannelReadConvAsync;
        g_MValue.measTimeChannelStartConv = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    case SensorMeasureStepChannelReadConvAsync:
    {
        uint8_t isConversionDone = 0;
        int32_t RawAdcValue = 0;
        errorCode += SensorMeasureChannelReadConvAsync(&isConversionDone, &RawAdcValue);
        if(isConversionDone != 0)
        {
            //未超时且转换完成
            adc_add_buffer_n(g_MValue.SensorChnlIndexCurrent, (float)RawAdcValue, 128);
            g_MValue.SensorChnlData[g_MValue.SensorChnlIndexCurrent].RawAdcValue = RawAdcValue;

            g_MValue.SensorMeasureStep = SensorMeasureStepChannelCalcAsync;
            g_MValue.measTimeChannelReadConv = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        }
        else
        {
            //转换未完成，继续检测
            g_MValue.SensorMeasureStep = SensorMeasureStepChannelCalcAsync;
            _delay_ms(10);
            if((g_MValue.ADS1220ConvTimeoutIn10ms--) == 0)
            {
                //ADC转换超时，在外部处理
                g_MValue.SensorChnlData[g_MValue.SensorChnlIndexCurrent].RawAdcValue = SENSOR_ERRORCODE_BAD_RawAdcValue;
                errorCode = 1;
            }
        }
        break;
    }
    case SensorMeasureStepChannelCalcAsync:
    {
        errorCode += SensorMeasureChannelCalcAsync(g_MValue.SensorChnlIndexCurrent);

        //判断当前通道是否完成需要的滤波次数转换
        if(g_MValue.ChnlFilterOrderCounter < g_MValue.ChnlFilterOrder)
        {
            g_MValue.SensorMeasureStep = SensorMeasureStepChannelStartConvAsync;
        }
        else
        {
            g_MValue.SensorChnlIndexCurrent++;
            //判断是否所有通道转换完成
            if(g_MValue.SensorChnlIndexCurrent < SENSOR_CHANNEL_INDEX_END)
            {
                //未完成所有通道
                g_MValue.SensorMeasureStep = SensorMeasureStepChannelInitConv;
            }
            else
            {
                //已完成所有通道，转换结束
                g_MValue.SensorMeasureStep = SensorMeasureStepCompleted;
                g_MValue.measTimeChannelCalc = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
            }
        }
        break;
    }
    case SensorMeasureStepCompleted:
    {
        //完成检测，重新开始
        g_MValue.SensorMeasureStep = SensorMeasureStepTempStartM1601Async;
        g_MValue.measTimeCompleted = RTC_IF_Convert_Tick2ms(RTC_IF_GetTimerValue() - g_MValue.measTimeStart);
        break;
    }
    default:
    {
        g_MValue.SensorMeasureStep = SensorMeasureStepCompleted;
        break;
    }
    }

    return errorCode;
}

int16_t SensorMeasureTempStartM1601Async(void)
{
    int16_t errorCode = 0;
    errorCode = M1601_Measure(&(g_MValue.M1601TempC));
    return errorCode;
}

int16_t SensorMeasureTempStartADS1220Async(void)
{
    int16_t errorCode = 0;

    //这里会检验ADS1220的通信是否正常
    errorCode = ADS1220_CheckHealth();
    if(errorCode == 0)
    {
        ADS1220_SetReg1EnableTemperatureSensor(ADS1220_TEMPSENSOR_ENABLE);
        //开始转换，ADS1220_Start内部有100ms延时，否则读出数据全部是0
        ADS1220_Start();
    }
    else
    {
    }

    return errorCode;
}

int16_t SensorMeasureTempReadADS1220Async(uint8_t* pIsConversionDone, float* pTemp)
{
    int16_t errorCode = 0;

    int32_t RawAdValue = 0;
    uint8_t isConversionDone = 0;

    errorCode += ADS1220_ReadRawAd_ByRData_CheckingPinDoutDrdy(&isConversionDone, &RawAdValue);
    *pIsConversionDone = isConversionDone;
    if(isConversionDone == 0)
    {
        errorCode = 0;
        return errorCode;
    }

    if(errorCode == 0)
    {
        int16_t result = RawAdValue >> 10;//24位数据的高14位为温度数据
        //判断第14位是0还是1
        if(result & 0x2000)
        {
            //如果是1表示负数，扩展高位为1
            result |= 0xE000;
        }
        else
        {
            //如果是0表示正数
            result = result ;
        }
        *pTemp = ((float)result) * 0.03125f;
    }
    return errorCode;
}

int16_t SensorMeasureTempCalcAmbientAsync(void)
{
    int16_t errorCode = 0;
    uint16_t tempSensor = eeprom_readint32((uint32_t*)&eepFactoryConfig.tempSensor);

    //4. 如果SHT40测量失败，则使用NTC作为温度传感器数据，最终数据存储在SHT4xTemp中
    //在（2）（3）的执行中，已经将原始的摄氏度值拷贝到g_MValue.NtcTempC，g_MValue.SHT4xTemp之中
    //首先检查使用哪个传感器的数据
    float TOffset = 0.0;
    float temperatureFinalOrigC = 0.0;
    float temperatureFinalCalibedC = 0.0;

    //计算temperatureFinalOrigC, temperatureFinalOrigF, temperatureFinalOrigK
    if((tempSensor == CONST_TEMPSENSOR_M1601) && (g_MValue.TempStatusM1601 == 0))
    {
        //如果选择M1601并且无异常
        temperatureFinalOrigC = g_MValue.M1601TempC;
    }
    else //if(tempSensor == CONST_TEMPSENSOR_ADS1220)
    {
        temperatureFinalOrigC = g_MValue.ADS1220TempC;
    }

    //计算temperatureFinalCalibedC, temperatureFinalCalibedF
    temperatureFinalCalibedC = temperatureFinalOrigC + TOffset;

    //进行数据限制,并赋值到响应寄存器
    if(temperatureFinalCalibedC < -40.00)
        temperatureFinalCalibedC = -40.00;
    if(temperatureFinalCalibedC > 125.00)
        temperatureFinalCalibedC = 125.00;

    g_MValue.FinalTempC = temperatureFinalCalibedC;

    return errorCode;
}
int16_t SensorMeasureChannelInitAsync(EnumSensorChannelIndex SensorChnlIndex)
{
    int16_t errorCode = 0;
    if(SensorChnlIndex>=SENSOR_CHANNEL_INDEX_END)
    {
        errorCode =1;
        return errorCode;
    }

    uint32_t RtdType = (uint32_t) eeprom_readint32((uint32_t*) & (eepChannelConfig.ItemConfigs[SensorChnlIndex].RtdType));
    STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG SensorTypeChannelAdcConfig;

    SensorTypeChannelAdcConfig = GetSensorTypeChannelAdcConfig(RtdType, SensorChnlIndex);
    ADS1220_SetReg0_Mux_Gain_BypassPGA(SensorTypeChannelAdcConfig.Mux, SensorTypeChannelAdcConfig.Gain, SensorTypeChannelAdcConfig.BypassPGA);
    return errorCode;
}

int16_t SensorMeasureChannelReadConvAsync(uint8_t* pIsConversionDone, int32_t* pRawAdValue)
{
    int16_t errorCode = 0;
    int32_t RawAdValue = 0;
    uint8_t isConversionDone = 0;

    errorCode += ADS1220_ReadRawAd_ByRData_CheckingPinDoutDrdy(&isConversionDone, &RawAdValue);
    *pIsConversionDone = isConversionDone;
    if(isConversionDone == 0)
    {
        errorCode = 0;
        return errorCode;
    }

    if(errorCode == 0)
    {
        //判断第24位是0还是1
        if(RawAdValue & 0x800000)
        {
            //如果是1表示负数，扩展高位为1
            RawAdValue |= 0xFF800000;
        }
        else
        {
            //如果是0表示正数
            RawAdValue = RawAdValue ;
        }
        *pRawAdValue = RawAdValue;
    }
    return errorCode;
}

int16_t SensorMeasureChannelCalcAsync(EnumSensorChannelIndex SensorChnlIndex)
{
    int16_t errorCode = 0;
    if(SensorChnlIndex>=SENSOR_CHANNEL_INDEX_END)
    {
        errorCode =1;
        return errorCode;
    }
    return errorCode;
}
