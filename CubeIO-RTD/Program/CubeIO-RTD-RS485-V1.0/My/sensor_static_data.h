#ifndef SENSOR_STATIC_DATA_H_
#define SENSOR_STATIC_DATA_H_

#include <stdint.h>
#include "eeprom.h"
#include "ads1220.h"

#define ADC_INPUT_RANGE_SMALL    					0
#define ADC_INPUT_RANGE_LARGE   					1

//------------------------------------------
//Sensor type define
//------------------------------------------
#define SENSORTYPE_NUM            40
#define SENSORTYPE_PT100_3851     0
#define SENSORTYPE_PT200_3851     1
#define SENSORTYPE_PT500_3851     2
#define SENSORTYPE_PT1000_3851    3

#define SENSORTYPE_PT100_3916     4
#define SENSORTYPE_PT200_3916     5
#define SENSORTYPE_PT500_3916     6
#define SENSORTYPE_PT1000_3916    7

#define SENSORTYPE_PT100_3920     8
#define SENSORTYPE_PT200_3920     9
#define SENSORTYPE_PT500_3920     10
#define SENSORTYPE_PT1000_3920    11

#define SENSORTYPE_PT100_3911     12
#define SENSORTYPE_PT200_3911     13
#define SENSORTYPE_PT500_3911     14
#define SENSORTYPE_PT1000_3911    15

#define SENSORTYPE_PT100_3928     16
#define SENSORTYPE_PT200_3928     17
#define SENSORTYPE_PT500_3928     18
#define SENSORTYPE_PT1000_3928    19

#define SENSORTYPE_PT100_3750     20
#define SENSORTYPE_PT200_3750     21
#define SENSORTYPE_PT500_3750     22
#define SENSORTYPE_PT1000_3750    23

#define SENSORTYPE_NI100_6180     24
#define SENSORTYPE_NI120_6180     25
#define SENSORTYPE_NI500_6180     26
#define SENSORTYPE_NI1000_6180    27

#define SENSORTYPE_NI100_6720     28
#define SENSORTYPE_NI120_6720     29
#define SENSORTYPE_NI500_6720     30
#define SENSORTYPE_NI1000_6720    31

#define SENSORTYPE_CU50_428       32
#define SENSORTYPE_CU100_428      33
#define SENSORTYPE_CU1000_428     34

#define SENSORTYPE_R600           35
#define SENSORTYPE_R4000          36

#define SENSORTYPE_PT50_3851      37
#define SENSORTYPE_END            37

//------------------------------------------
//Polyfit params define
//------------------------------------------
////------------------------------------------
////Polyfit params define for Wheatstone bridge leg=300ohm
////------------------------------------------
//############################
//# SensorName:RTD_PT100_3851
//############################
extern const float COFF_RTD_PT100_3851[3][5];
extern const int32_t SEGMENT_RTD_PT100_3851[2];


//############################
//# SensorName:RTD_PT200_3851
//############################
extern const float COFF_RTD_PT200_3851[3][5];
extern const int32_t SEGMENT_RTD_PT200_3851[2];

//############################
//# SensorName:RTD_PT500_3851
//############################
extern const float COFF_RTD_PT500_3851[3][5];
extern const int32_t SEGMENT_RTD_PT500_3851[2];

//############################
//# SensorName:RTD_PT1000_3851
//############################
extern const float COFF_RTD_PT1000_3851[3][5];
extern const int32_t SEGMENT_RTD_PT1000_3851[2];


//############################
//# SensorName:RTD_PT100_3916
//############################
extern const float COFF_RTD_PT100_3916[2][6];
extern const int32_t SEGMENT_RTD_PT100_3916[1];

//############################
//# SensorName:RTD_PT200_3916
//############################
extern const float COFF_RTD_PT200_3916[2][6];
extern const int32_t SEGMENT_RTD_PT200_3916[1];

//############################
//# SensorName:RTD_PT500_3916
//############################
extern const float COFF_RTD_PT500_3916[3][5];
extern const int32_t SEGMENT_RTD_PT500_3916[2];

//############################
//# SensorName:RTD_PT1000_3916
//############################
extern const float COFF_RTD_PT1000_3916[3][5];
extern const int32_t SEGMENT_RTD_PT1000_3916[2];

//############################
//# SensorName:RTD_PT100_3920
//############################
extern const float COFF_RTD_PT100_3920[3][5];
extern const int32_t SEGMENT_RTD_PT100_3920[2];

//############################
//# SensorName:RTD_PT200_3920
//############################
extern const float COFF_RTD_PT200_3920[3][5];
extern const int32_t SEGMENT_RTD_PT200_3920[2];

//############################
//# SensorName:RTD_PT500_3920
//############################
extern const float COFF_RTD_PT500_3920[2][6];
extern const int32_t SEGMENT_RTD_PT500_3920[1];

//############################
//# SensorName:RTD_PT1000_3920
//############################
extern const float COFF_RTD_PT1000_3920[3][5];
extern const int32_t SEGMENT_RTD_PT1000_3920[2];


//############################
//# SensorName:RTD_PT100_3911
//############################
extern const float COFF_RTD_PT100_3911[3][5];
extern const int32_t SEGMENT_RTD_PT100_3911[2];

//############################
//# SensorName:RTD_PT200_3911
//############################
extern const float COFF_RTD_PT200_3911[3][5];
extern const int32_t SEGMENT_RTD_PT200_3911[2];
//############################
//# SensorName:RTD_PT500_3911
//############################
extern const float COFF_RTD_PT500_3911[3][5];
extern const int32_t SEGMENT_RTD_PT500_3911[2];

//############################
//# SensorName:RTD_PT1000_3911
//############################
extern const float COFF_RTD_PT1000_3911[3][5];
extern const int32_t SEGMENT_RTD_PT1000_3911[2];


//############################
//# SensorName:RTD_PT100_3928
//############################
extern const float COFF_RTD_PT100_3928[3][5];
extern const int32_t SEGMENT_RTD_PT100_3928[2];

//############################
//# SensorName:RTD_PT200_3928
//############################
extern const float COFF_RTD_PT200_3928[3][5];
extern const int32_t SEGMENT_RTD_PT200_3928[2];

//############################
//# SensorName:RTD_PT500_3928
//############################
extern const float COFF_RTD_PT500_3928[4][5];
extern const int32_t SEGMENT_RTD_PT500_3928[3];

//############################
//# SensorName:RTD_PT1000_3928
//############################
extern const float COFF_RTD_PT1000_3928[3][5];
extern const int32_t SEGMENT_RTD_PT1000_3928[2];

//############################
//# SensorName:RTD_PT100_3750
//############################
extern const float COFF_RTD_PT100_3750[2][6];
extern const int32_t SEGMENT_RTD_PT100_3750[1];

//############################
//# SensorName:RTD_PT200_3750
//############################
extern const float COFF_RTD_PT200_3750[3][5];
extern const int32_t SEGMENT_RTD_PT200_3750[2];

//############################
//# SensorName:RTD_PT500_3750
//############################
extern const float COFF_RTD_PT500_3750[3][5];
extern const int32_t SEGMENT_RTD_PT500_3750[2];

//############################
//# SensorName:RTD_PT1000_3750
//############################
extern const float COFF_RTD_PT1000_3750[2][6];
extern const int32_t SEGMENT_RTD_PT1000_3750[1];


//Ni6180
//############################
//# SensorName:RTD_NI100_6180
//############################
extern const float COFF_RTD_NI100_6180[2][5];
extern const int32_t SEGMENT_RTD_NI100_6180[1];


//############################
//# SensorName:RTD_NI120_6180
//############################
extern const float COFF_RTD_NI120_6180[2][5];
extern const int32_t SEGMENT_RTD_NI120_6180[1];

//############################
//# SensorName:RTD_NI500_6180
//############################
extern const float COFF_RTD_NI500_6180[2][5];
extern const int32_t SEGMENT_RTD_NI500_6180[1];


//############################
//# SensorName:RTD_NI1000_6180
//############################
extern const float COFF_RTD_NI1000_6180[2][5];
extern const int32_t SEGMENT_RTD_NI1000_6180[1];


//############################
//# SensorName:RTD_NI100_6720
//############################
extern const float COFF_RTD_NI100_6720[2][5];
extern const int32_t SEGMENT_RTD_NI100_6720[1];

//############################
//# SensorName:RTD_NI120_6720
//############################
extern const float COFF_RTD_NI120_6720[2][5];
extern const int32_t SEGMENT_RTD_NI120_6720[1];

//############################
//# SensorName:RTD_NI500_6720
//############################
extern const float COFF_RTD_NI500_6720[2][5];
extern const int32_t SEGMENT_RTD_NI500_6720[1];

//############################
//# SensorName:RTD_NI1000_6720
//############################
extern const float COFF_RTD_NI1000_6720[2][5];
extern const int32_t SEGMENT_RTD_NI1000_6720[1];

//############################
//# SensorName:RTD_CU50_428
//############################
extern const float COFF_RTD_CU50_428[2][5];
extern const int32_t SEGMENT_RTD_CU50_428[1];


//############################
//# SensorName:RTD_CU100_428
//############################
extern const float COFF_RTD_CU100_428[2][5];
extern const int32_t SEGMENT_RTD_CU100_428[1];


//############################
//# SensorName:RTD_CU1000_428
//############################
extern const float COFF_RTD_CU1000_428[2][5];
extern const int32_t SEGMENT_RTD_CU1000_428[1];

//############################
//# SensorName:RTD_R600
//############################
extern const float COFF_RTD_R600[2][6];
extern const int32_t SEGMENT_RTD_R600[1];


//############################
//# SensorName:RTD_R4000
//############################
extern const float COFF_RTD_R4000[3][5];
extern const int32_t SEGMENT_RTD_R4000[2];


//############################
//# SensorName:RTD_PT50_3851
//############################
extern const float COFF_RTD_PT50_3851[3][5];
extern const int32_t SEGMENT_RTD_PT50_3851[2];


//------------------------------------------
//ADC works in two ranges, one is +/-100mV
//the other is +/-1000mV, For two ranges, we have to
//calibrate seperately to ensure the precision
//------------------------------------------
extern const int32_t PROGMEM_CALIBPOINT_IDEAL_VALUE[SENSORTYPE_NUM][2];
//------------------------------------------
//ADC code range, this is ideal value, for use of OVER RANGE detection
//for example , PT100 from -200 ~850 degree, values varies from (-2123354, 7431804)
//------------------------------------------
extern const int32_t PROGMEM_SENSOR_ADC_RANGE[SENSORTYPE_NUM][2];
//------------------------------------------
//Sensor range, for use of OVER RANGE detection
//The value is (original_value * 10)
//------------------------------------------
extern const int32_t PROGMEM_SENSOR_RESULT_RANGE[SENSORTYPE_NUM][2];

int32_t adcCalibSystemOffset(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t systemOffset, int32_t RawAdcValue);
int32_t adcTwoPointCalib(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t RawAdcValue);
float adcCalcSensorValue(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t RawAdcValueCalibed);
int32_t adcCalcEngCalibedSensorValue(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t Value);
int32_t adcCheckAdcOverRange(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t RawAdcValue);
int32_t adcCheckSensorValueOverRange(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t Value);

typedef enum EnumSensorChannelIndex
{
    SENSOR_CHANNEL_INDEX_0 = 0x0000,
    SENSOR_CHANNEL_INDEX_1 = 0x0001,
    SENSOR_CHANNEL_INDEX_END = SENSOR_CHANNEL_SIZE_THISDEVICE,
} EnumSensorChannelIndex;

//--------------------------------------
//Structs to hold channel status
//--------------------------------------
//Value for STRUCT_SENSOR_CHANNEL_DATA.RawAdcValue and STRUCT_SENSOR_CHANNEL_DATA.Value
#define SENSOR_CHNL_STATUS_SNROHM_OVHI_VALUE 	0xFFFF
#define SENSOR_CHNL_STATUS_SNROHM_OVLO_VALUE 	0xFFFF
#define SENSOR_CHNL_STATUS_SNR_OVHI_VALUE 		-32768
#define SENSOR_CHNL_STATUS_SNR_OVLO_VALUE 		-32768
#define SENSOR_CHNL_STATUS_SNR_ERROR_VALUE 		-32768
#define SENSOR_CHNL_STATUS_ADC_OVHI_VALUE 		8388607
#define SENSOR_CHNL_STATUS_ADC_OVLO_VALUE 		-8388608
#define SENSOR_CHNL_STATUS_ADC_ERROR_VALUE 		-8388608

//Value for STRUCT_SENSOR_CHANNEL_DATA.SensorChannelStatus
typedef enum EnumSensorChannelStatus
{
    SENSOR_CHNL_STATUS_SNR_BURNOUT= 0x01,
    SENSOR_CHNL_STATUS_ADC_OVHI   = 0x02,
    SENSOR_CHNL_STATUS_ADC_OVLO   = 0x04,
    SENSOR_CHNL_STATUS_SNR_OVHI  =  0x08,
    SENSOR_CHNL_STATUS_SNR_OVLO  =  0x10,
} EnumSensorChannelStatus;

typedef struct STRUCT_SENSOR_CHANNEL_DATA
{
    EnumSensorChannelStatus SensorChannelStatus;
    int32_t RawAdcValue;
    int32_t RawAdcValueCalibed;
    int32_t Value;
    int32_t ValueOffseted;
} STRUCT_SENSOR_CHANNEL_DATA;

//传感器类型对应的ADC配置
typedef struct STRUCT_SENSORTYPE_ADCCONFIG
{
    EnumAds1220Gain Gain;
    EnumAds1220BypassPGA BypassPGA;
} STRUCT_SENSORTYPE_ADCCONFIG;
extern const STRUCT_SENSORTYPE_ADCCONFIG PROGMEM_STRUCT_SENSORTYPE_ADCCONFIG[SENSORTYPE_NUM];

//传感器类型与通道的组合对应的ADC配置
typedef struct STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG
{
    EnumAds1220Mux Mux;
    EnumAds1220Gain Gain;
    EnumAds1220BypassPGA BypassPGA;
    uint8_t Reserved0;
    uint8_t Reserved1;
} STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG;
extern STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG GetSensorTypeChannelAdcConfig(uint32_t RtdType, EnumSensorChannelIndex SensorChannelIndex);

#endif
