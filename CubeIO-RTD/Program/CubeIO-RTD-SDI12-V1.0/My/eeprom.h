#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

#define CONST_SDI12ADDRESS_DEFAULT 		    '0'

#define CONST_TEMPUNIT_C                 'C'
#define CONST_TEMPUNIT_F                 'F'
#define CONST_TEMPUNIT_DEFAULT           CONST_TEMPUNIT_C

#define CONST_TEMPOFFSET_MIN              ((float)-10.00)   //-10.00
#define CONST_TEMPOFFSET_MAX              ((float)10.00)    //+10.00
#define CONST_TEMPOFFSET_DEFAULT          ((float)0.00)     //0.00

#define CONST_FILTERWINDOW_DISABLED	 0
#define CONST_FILTERWINDOW_MIN		 	 0
#define CONST_FILTERWINDOW_MAX 			 16
#define CONST_FILTERWINDOW_DEFAULT 	 16

#define CONST_ASYNCACQINTERVAL_DISABLED	 0
#define CONST_ASYNCACQINTERVAL_MIN		 	 10   //最小10秒
#define CONST_ASYNCACQINTERVAL_MAX 			 ((uint16_t)12*60*60) //最大12小时
#define CONST_ASYNCACQINTERVAL_DEFAULT 	 CONST_ASYNCACQINTERVAL_DISABLED

typedef struct STRUCT_USERCONFIG
{
    uint32_t sdi12Address;
    uint32_t sn[2];//8 digital SN
    uint32_t temperatureUnit;
    float temperatureOffset;
    uint32_t filterWindow;
    uint32_t asyncAcqInterval;
    uint32_t reserved5;
    uint32_t reserved6;
    uint32_t reserved7;
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_USERCONFIG;


#define CONST_TEMPSOURCE_SENSOR        0
#define CONST_TEMPSOURCE_MANUALINPUT   1
#define CONST_TEMPSOURCE_DEFAULT       CONST_TEMPSOURCE_SENSOR

#define CONST_TEMP_MANUALINPUT_MIN      ((float)-5.00)
#define CONST_TEMP_MANUALINPUT_MAX      ((float)85.00)
#define CONST_TEMP_MANUALINPUT_DEFAULT  ((float)25.00)

#define CONST_ECCALIBMODE_CONSTANT	 0
#define CONST_ECCALIBMODE_SOLUTION	 1
#define CONST_ECCALIBMODE_DEFAULT 	 0

//#define CONST_CELLCONSTANT_NUM        4
//#define CONST_CELLCONSTANT_0P01	   		0
//#define CONST_CELLCONSTANT_0P1	   		1
#define CONST_CELLCONSTANT_1P0 	   		2
//#define CONST_CELLCONSTANT_10P0 	 		3
#define CONST_CELLCONSTANT_DEFAULT	 	CONST_CELLCONSTANT_1P0

#define CONST_CELLCONSTANTVALUE_MIN	  			((float)0.0001) //对于不同的电极常数指示，数值具有不同的意义
#define CONST_CELLCONSTANTVALUE_MAX	  			((float)6.0000) //对于不同的电极常数指示，数值具有不同的意义
#define CONST_CELLCONSTANTVALUE_DEFAULT	  	((float)1.0000) //对于不同的电极常数指示，数值具有不同的意义

#define CONST_ECREFTEMPERATURE_05C	  		((float)5.00)
#define CONST_ECREFTEMPERATURE_25C	  		((float)25.00)
#define CONST_ECREFTEMPERATURE_30C	  		((float)30.00)
#define CONST_ECREFTEMPERATURE_MIN	  		CONST_ECREFTEMPERATURE_05C
#define CONST_ECREFTEMPERATURE_MAX	  		CONST_ECREFTEMPERATURE_30C
#define CONST_ECREFTEMPERATURE_DEFAULT	  CONST_ECREFTEMPERATURE_25C

#define CONST_ECTCOMPENSATIONMODE_DISABLE			  0
#define CONST_ECTCOMPENSATIONMODE_LINEAR				1
//#define CONST_ECTCOMPENSATIONMODE_PUREWATER	  2
#define CONST_ECTCOMPENSATIONMODE_DEFAULT	    CONST_ECTCOMPENSATIONMODE_LINEAR

#define CONST_COEF_ECTCOMPENSATION_MIN      			((float)0.000)
#define CONST_COEF_ECTCOMPENSATION_MAX      			((float)10.000)
#define CONST_COEF_ECTCOMPENSATION_DEFAULT  			((float)2.000)

#define CONST_COEF_TDS_MIN      			((float)0.001)
#define CONST_COEF_TDS_MAX      			((float)5.000)
#define CONST_COEF_TDS_DEFAULT  			((float)0.500)

#define CONST_COEF_SALINITY_MIN      	((float)0.001)
#define CONST_COEF_SALINITY_MAX      	((float)5.000)
#define CONST_COEF_SALINITY_DEFAULT  	((float)0.550)

typedef struct STRUCT_SENSORCONFIG
{
    uint32_t tempSource;
    float tempManualInput;
    float ecRefTemperature;
    uint32_t ecTCompensationMode;
    uint32_t ecCalibMode;
    float cellConstantValue;
    float coefECTCompensation;
    float coefTDS;
    float coefSalinity;
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_SENSORCONFIG;

#define CONST_SENSORTYPE_CubeIoRtd  					  0x00

#define CONST_TEMPSENSOR_ADS1220 					0
#define CONST_TEMPSENSOR_M1601  					1

typedef struct STRUCT_FACTORYCONFIG
{
    uint32_t productInfo[8];//32 byte SDI12 info
    uint32_t sensorType;
    uint32_t tempSensor;
    uint32_t cellConstant;//本产品暂不使用这个参数
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_FACTORYCONFIG;

#define CONST_CHNL_FILTERORDER_00    0
#define CONST_CHNL_FILTERORDER_03    3
#define CONST_CHNL_FILTERORDER_16    16
#define CONST_CHNL_FILTERORDER_MIN   CONST_CHNL_FILTERORDER_00
#define CONST_CHNL_FILTERORDER_MAX   CONST_CHNL_FILTERORDER_16
#define CONST_CHNL_FILTERORDER_DEF   CONST_CHNL_FILTERORDER_03

#define CONST_CHNL_TOFFSET_MIN       ((int16_t)-1000)   //-10.00
#define CONST_CHNL_TOFFSET_MAX       ((int16_t)1000)    //+10.00
#define CONST_CHNL_TOFFSET_DEF       ((int16_t)0)     //0.00

#define CONST_CHNL_ENABLE_DISABLE   0
#define CONST_CHNL_ENABLE_ENABLED   1

#define CONST_CHNL_CJC_DISABLE   	 0
#define CONST_CHNL_CJC_ENABLED   	 1

//Zero Full Point Calib Param
typedef struct STRUCT_CHANNEL_ITEM_CONFIG
{
    uint32_t RtdType;
    uint32_t FilterOrder;
    int32_t TOffset;
    uint32_t ChnlEnable;
    uint32_t CjcEnable;
    int32_t CalibPt_AdcInputRangeSmall_Lower;//ADC_INPUT_RANGE_SMALL
    int32_t CalibPt_AdcInputRangeSmall_Upper;
    int32_t CalibPt_AdcInputRangeLarge_Lower;//ADC_INPUT_RANGE_LARGE
    int32_t CalibPt_AdcInputRangeLarge_Upper;
} STRUCT_CHANNEL_ITEM_CONFIG;

#define SENSOR_CHANNEL_SIZE_MAXSUPPORT				16  //本程序支持的传感器输入通道数量
#define SENSOR_CHANNEL_SIZE_THISDEVICE		    2   //本设备实际的传感器输入通道数量

typedef struct STRUCT_CHANNEL_CONFIG
{
    STRUCT_CHANNEL_ITEM_CONFIG ItemConfigs[SENSOR_CHANNEL_SIZE_MAXSUPPORT];
    uint32_t crc16; // crc-16 high byte in low address
} STRUCT_CHANNEL_CONFIG;

extern STRUCT_USERCONFIG eepUserConfig;
extern STRUCT_SENSORCONFIG eepSensorConfig;
extern STRUCT_FACTORYCONFIG eepFactoryConfig;
extern STRUCT_CHANNEL_CONFIG eepChannelConfig;

extern void eeprom_init(void);
extern void eeprom_reset_factory(void);
extern void eeprom_reset_calibration(void);

#endif /* EEPROM_H_ */
