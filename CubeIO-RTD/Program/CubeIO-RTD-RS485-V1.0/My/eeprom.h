#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

typedef struct STRUCT_USARTCOMM
{
    uint32_t address; // slave number
    uint32_t baudrate; // baud rate code
    uint32_t protocol; // communication protocol
    uint32_t parity; // none(0),even(1),odd(2) check
    uint32_t dataBits; // data bits
    uint32_t stopBits; // stop bits
    uint32_t responseDealy; // Slave delay "responseDealy*10ms", then response to the master request
    uint32_t activeOutputInterval;//salve active output interval, unit is second
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_USARTCOMM;

typedef struct STRUCT_MANUINFO
{
    uint32_t yyyy; // manufacture date
    uint32_t mmdd;
    uint32_t hhmm;
    uint32_t productid; // series number
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_MANUINFO;

typedef struct STRUCT_USERTAG
{
    uint32_t sn[2];
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_USERTAG;

//MDK Byte Order: 0,1,2,3(内存中顺序)
#define CONST_FLOATBYTEORDER_IEEE754_BIGENDIAN            0 //3,2,1,0(内存中顺序), 原来ModbusPoll中的Float-Inverse
#define CONST_FLOATBYTEORDER_IEEE754_LITTLEENDIAN         1 //0,1,2,3(内存中顺序)
#define CONST_FLOATBYTEORDER_IEEE754_BIGENDIANWORDSWAP    2 //2,3,0,1(内存中顺序)
#define CONST_FLOATBYTEORDER_IEEE754_LITTLEENDIANWORDSWAP 3 //1,0,3,2(内存中顺序), 原来ModbusPoll中的Float
#define CONST_FLOATBYTEORDER_END                          CONST_FLOATBYTEORDER_IEEE754_LITTLEENDIANWORDSWAP
#define CONST_FLOATBYTEORDER_DEFAULT                      CONST_FLOATBYTEORDER_IEEE754_LITTLEENDIANWORDSWAP

typedef struct STRUCT_USERCONFIG
{
    uint32_t floatByteOrder;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t reserved4;
    uint32_t reserved5;
    uint32_t reserved6;
    uint32_t reserved7;
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_USERCONFIG;

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
    int32_t calibPoint100mv_1;
    int32_t calibPoint100mv_2;
    int32_t calibPoint1000mv_1;
    int32_t calibPoint1000mv_2;
} STRUCT_CHANNEL_ITEM_CONFIG;

#define SENSOR_CHANNEL_SIZE_MAXSUPPORT				16  //本程序支持的传感器输入通道数量
#define SENSOR_CHANNEL_SIZE_THISDEVICE		    2   //本设备实际的传感器输入通道数量

typedef struct STRUCT_CHANNEL_CONFIG
{
    STRUCT_CHANNEL_ITEM_CONFIG ItemConfigs[SENSOR_CHANNEL_SIZE_MAXSUPPORT];
    uint32_t crc16; // crc-16 high byte in low address
} STRUCT_CHANNEL_CONFIG;

#define CONST_SENSORTYPE_CubeIoRtd     				0x00

#define CONST_TEMPSENSOR_ADS1220 					0
#define CONST_TEMPSENSOR_M1601  					1

typedef struct STRUCT_FACTORYCONFIG
{
    uint32_t sensorType;
    uint32_t tempSensor;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t crc; // crc-16 high byte in low address
} STRUCT_FACTORYCONFIG;

extern STRUCT_USARTCOMM eepUsart;
extern STRUCT_MANUINFO eepManuInfo;
extern STRUCT_USERTAG eepUserTag;
extern STRUCT_USERCONFIG eepUserConfig;
extern STRUCT_CHANNEL_CONFIG eepChannelConfig;
extern STRUCT_FACTORYCONFIG eepFactoryConfig;

extern void eeprom_init(void);
extern void eeprom_reset_factory(void);
extern void eeprom_reset_calibration(void);

#endif /* EEPROM_H_ */
