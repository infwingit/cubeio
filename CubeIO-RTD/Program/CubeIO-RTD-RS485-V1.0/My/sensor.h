#ifndef SENSOR_H_
#define SENSOR_H_

#include "main.h"
#include "eeprom.h"
#include "sensor_static_data.h"

#define SENSOR_STATUS_ERROR          			1
#define SENSOR_STATUS_OK             			0

#define SENSOR_ERRORCODE_BROKEN_ForModbus_Float_Neg32768     				-32768
#define SENSOR_ERRORCODE_BROKEN_ForModbus_Float_Neg1         				-1
#define SENSOR_ERRORCODE_NOT_SUPPORTED_ForModbus_Float_Neg32768  		-32765

#define SENSOR_ERRORCODE_BROKEN_ForModbus_Int16         -32768
#define SENSOR_ERRORCODE_NOT_SUPPORTED_ForModbus_Int16  -32765

#define SENSOR_ERRORCODE_BROKEN_ForModbus_Int32         0xFFFFFFFF
#define SENSOR_ERRORCODE_NOT_SUPPORTED_ForModbus_Int32  0xFFFFFFFF

#define SENSOR_ERRORCODE_BROKEN_ForSDI12         ((float)-9999)
#define SENSOR_ERRORCODE_NOT_SUPPORTED_ForSDI12  ((float)-9996)

#define SENSOR_ERRORCODE_BAD_RawAdcValue     			-8388607

#define CONST_FACTORYRESET_STATUS_DONE       	0
#define CONST_FACTORYRESET_STATUS_PROCESSING  1

#define CONST_SYSTEMRESET_STATUS_DONE       	0
#define CONST_SYSTEMRESET_STATUS_PROCESSING   1

#define CONST_SELFCALIB_STATUS_DONE       	  0
#define CONST_SELFCALIB_STATUS_PROCESSING     1
#define CONST_SELFCALIB_STATUS_FAIL           2

#define CONST_CALIBRESET_STATUS_DONE       	  0
#define CONST_CALIBRESET_STATUS_PROCESSING    1

//以下用于用户标定时的错误信息返回
#define CONST_CALIBRESULT_SUCCESS             0
#define CONST_CALIBRESULT_ERROR_TARGETVALUE   1
#define CONST_CALIBRESULT_ERROR_SENSOR        2

typedef enum EnumSensorMeasureStep
{
    SensorMeasureStepDeInit = 0,
    SensorMeasureStepInit,
    SensorMeasureStepTempStartM1601Async,
    SensorMeasureStepTempStartADS1220Async,//这里会检验ADS1220的通信是否正常
    SensorMeasureStepTempReadADS1220Async,
    SensorMeasureStepTempCalcAmbientAsync,
    SensorMeasureStepChannelPrepareConv,
    SensorMeasureStepChannelInitConv,
    SensorMeasureStepChannelWaitStableAsync,
    SensorMeasureStepChannelStartConvAsync,
    SensorMeasureStepChannelReadConvAsync,
    SensorMeasureStepChannelCalcAsync,
    SensorMeasureStepCompleted,
} EnumSensorMeasureStep;

typedef enum EnumSensorMeasureStatus
{
    SensorMeasureStatusStart = 0,
    SensorMeasureStatusDone,
    SensorMeasureStatusFail,
} EnumSensorMeasureStatus;

typedef enum EnumChannelDataMode
{
    MEASUREMODE_TEMP = 0,
    MEASUREMODE_VOL,
    MEASUREMODE_VOL_CALIB_SYSOFFSET,
} EnumMeasurementMode;

typedef struct MEASURE_VALUE
{
    uint16_t SensorStatus;
    EnumSensorMeasureStep SensorMeasureStep;

    //周期性采集相关
    uint8_t SensorDeInitCounter;//对反向初始化次数进行计数，用于对传感器电源断电的时间进行控制
    EnumSensorMeasureStatus SensorMeasureStatus;//测量状态标志，供主机读取用，无其他流程控制作用
    uint16_t ADS1220ConvTimeoutIn10ms;//判断是否达到了需要的滤波次数

    //环境温度相关，板上温度传感器，用于热电偶冷端补偿
    int16_t TempStatusADS1220;//0:正常， >1:异常
    int16_t TempStatusM1601;//0:正常， >1:异常
    float ADS1220TempC;
    float M1601TempC;
    float FinalTempC;

    //通道数据转换相关
    EnumSensorChannelIndex SensorChnlIndexCurrent;//当前正在转换的通道号
    uint32_t ChnlFilterOrder;//当前正在转换的通道，滤波次数
    uint32_t ChnlFilterOrderCounter;//当前正在转换的通道，滤波次数计数
    int32_t AdcSystemOffset_100mv;//保留，暂时不用
    int32_t AdcSystemOffset_1000mv;//保留，暂时不用
    int32_t EnvTempAdc;//热电偶冷端补偿用，热电阻不用
    float EnvTemp;//热电偶冷端补偿用，热电阻不用
    STRUCT_SENSOR_CHANNEL_DATA SensorChnlData[SENSOR_CHANNEL_SIZE_MAXSUPPORT];

    //操作状态
    uint8_t FactoryResetStatus;
    uint8_t SystemResetStatus;
    uint8_t SelfCalibStatus;
    uint8_t CalibResetStatus;

    //以下记录每个采集步骤的执行时间
    uint32_t measTimeStart;
    uint32_t measTimeTempStartM1601;
    uint32_t measTimeTempStartADS1220;
    uint32_t measTimeTempReadADS1220;
    uint32_t measTimeTempCalcAmbient;
    uint32_t measTimeChannelPrepareConv;
    uint32_t measTimeChannelInitConv;
    uint32_t measTimeChannelWaitStable;
    uint32_t measTimeChannelStartConv;
    uint32_t measTimeChannelReadConv;
    uint32_t measTimeChannelCalc;
    uint32_t measTimeCompleted;
} MEASURE_VALUE;
extern MEASURE_VALUE g_MValue;

extern int16_t SensorInitPowerOn(void);
extern int16_t SensorInit(void);
extern int16_t SensorDeInit(void);

extern int16_t SensorMeasureAsync(EnumSensorMeasureStep* pSensorMeasureStepDone);
extern int16_t SensorMeasureTempStartM1601Async(void);
extern int16_t SensorMeasureTempStartADS1220Async(void);
extern int16_t SensorMeasureTempReadADS1220Async(uint8_t* pIsConversionDone, float* pTemp);
extern int16_t SensorMeasureTempCalcAmbientAsync(void);
extern int16_t SensorMeasureChannelInitAsync(EnumSensorChannelIndex SensorChnlIndex);
extern int16_t SensorMeasureChannelReadConvAsync(uint8_t* pIsConversionDone, int32_t* pRawAdValue);
extern int16_t SensorMeasureChannelCalcAsync(EnumSensorChannelIndex SensorChnlIndex);

#endif /* GLOBALS_H_ */

