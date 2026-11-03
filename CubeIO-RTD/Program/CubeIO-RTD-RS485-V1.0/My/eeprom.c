#include "eeprom.h"
#include "utils.h"
#include "sensor.h"
#include "hal_eeprom.h"
#include "sensor_static_data.h"

#define EEPROM_BASE_ADDR 0x08080000

extern STRUCT_USARTCOMM eepUsart;
extern STRUCT_MANUINFO eepManuInfo;
extern STRUCT_USERTAG eepUserTag;
extern STRUCT_USERCONFIG eepUserConfig;
extern STRUCT_CHANNEL_CONFIG eepChannelConfig;
extern STRUCT_FACTORYCONFIG eepFactoryConfig;

#define EEPROM_ADDR0 (EEPROM_BASE_ADDR+0)
STRUCT_USARTCOMM eepUsart  __attribute__((at(EEPROM_ADDR0)));

#define EEPROM_ADDR1 (EEPROM_ADDR0+sizeof(STRUCT_USARTCOMM))
STRUCT_MANUINFO eepManuInfo  __attribute__((at(EEPROM_ADDR1)));

#define EEPROM_ADDR2 (EEPROM_ADDR1+sizeof(STRUCT_MANUINFO))
STRUCT_USERTAG eepUserTag __attribute__((at(EEPROM_ADDR2)));

#define EEPROM_ADDR3 (EEPROM_ADDR2+sizeof(STRUCT_USERTAG))
STRUCT_USERCONFIG eepUserConfig __attribute__((at(EEPROM_ADDR3)));

#define EEPROM_ADDR4 (EEPROM_ADDR3+sizeof(STRUCT_USERCONFIG))
STRUCT_CHANNEL_CONFIG eepChannelConfig __attribute__((at(EEPROM_ADDR4)));

#define EEPROM_ADDR5 (EEPROM_ADDR4+sizeof(STRUCT_CHANNEL_CONFIG))
STRUCT_FACTORYCONFIG eepFactoryConfig __attribute__((at(EEPROM_ADDR5)));

const char* CONST_DEFAULT_SN = "INFWIN  ";

void eeprom_init(void)
{
    if(crc32EepromCheck((uint32_t*) &eepUsart, sizeof(STRUCT_USARTCOMM) / sizeof(uint8_t)))
    {
        eeprom_writeint32(&eepUsart.address, 1); //默认为0
        eeprom_writeint32(&eepUsart.baudrate, SERCOMM_BAUDRATE_9600); //9600
        eeprom_writeint32(&eepUsart.protocol, SERCOMM_PROTOCOL_MODRTU); //modbus rtu
        eeprom_writeint32(&eepUsart.parity, SERCOMM_PARITY_NONE); //modbus rtu
        eeprom_writeint32(&eepUsart.dataBits, SERCOMM_DATABITS_8); //modbus rtu
        eeprom_writeint32(&eepUsart.stopBits, SERCOMM_STOPBITS_1); //modbus rtu
        eeprom_writeint32(&eepUsart.responseDealy, SERCOMM_RESPONSEDELAY_0MS); //response delay time
        eeprom_writeint32(&eepUsart.activeOutputInterval, SERCOMM_ACTVOUTPUTINTERVAL_0S);
        crc32EepromAppend((uint32_t*) &eepUsart, sizeof(STRUCT_USARTCOMM) / sizeof(char)); //重新计算crc}
    }

    if(crc32EepromCheck((uint32_t*) &eepManuInfo, sizeof(STRUCT_MANUINFO) / sizeof(char)))
    {
        eeprom_writeint32(&eepManuInfo.yyyy, 2026);
        eeprom_writeint32(&eepManuInfo.hhmm, 0x0A0A);
        eeprom_writeint32(&eepManuInfo.mmdd, 0x0210);
        eeprom_writeint32(&eepManuInfo.productid, 6866);
        crc32EepromAppend((uint32_t*) &eepManuInfo, sizeof(STRUCT_MANUINFO) / sizeof(char));
    }

    if(crc32EepromCheck((uint32_t*) &eepUserTag, sizeof(STRUCT_USERTAG) / sizeof(uint8_t)))
    {
        for(uint8_t i = 0; i < 2; i++)
        {
            uint16_t hi16 = MAKE_16BIT(CONST_DEFAULT_SN[4 * i], CONST_DEFAULT_SN[4 * i + 1]);
            uint16_t lo16 = MAKE_16BIT(CONST_DEFAULT_SN[4 * i + 2], CONST_DEFAULT_SN[4 * i + 3]);
            uint32_t value = MAKE_32BIT(hi16, lo16);
            eeprom_writeint32(&eepUserTag.sn[i], value);
        }
        crc32EepromAppend((uint32_t*) &eepUserTag, sizeof(STRUCT_USERTAG) / sizeof(uint8_t));
    }

    if(crc32EepromCheck((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t)))
    {
        eeprom_writeint32((uint32_t*) & (eepUserConfig.floatByteOrder), CONST_FLOATBYTEORDER_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved1), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved2), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved3), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved4), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved5), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved6), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved7), 0);
        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));
    }

    if(crc32EepromCheck((uint32_t*) &eepChannelConfig, sizeof(STRUCT_CHANNEL_CONFIG) / sizeof(uint8_t)))
    {
        for (uint8_t i = 0; i < SENSOR_CHANNEL_SIZE_MAXSUPPORT; i++)
        {
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].RtdType), SENSORTYPE_PT100_3851);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].FilterOrder), CONST_CHNL_FILTERORDER_DEF);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].TOffset), CONST_CHNL_TOFFSET_DEF);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].ChnlEnable), CONST_CHNL_ENABLE_ENABLED);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].CjcEnable), CONST_CHNL_CJC_ENABLED);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].CalibPt_AdcInputRangeSmall_Lower), PROGMEM_CALIBPOINT_IDEAL_VALUE[ADC_INPUT_RANGE_SMALL][0]);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].CalibPt_AdcInputRangeSmall_Upper), PROGMEM_CALIBPOINT_IDEAL_VALUE[ADC_INPUT_RANGE_SMALL][1]);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].CalibPt_AdcInputRangeLarge_Lower), PROGMEM_CALIBPOINT_IDEAL_VALUE[ADC_INPUT_RANGE_LARGE][0]);
            eeprom_writeint32((uint32_t*) & (eepChannelConfig.ItemConfigs[i].CalibPt_AdcInputRangeLarge_Upper), PROGMEM_CALIBPOINT_IDEAL_VALUE[ADC_INPUT_RANGE_LARGE][1]);
        }
        crc32EepromAppend((uint32_t*) &eepChannelConfig, sizeof(STRUCT_CHANNEL_CONFIG) / sizeof(uint8_t));
    }

    if(crc32EepromCheck((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t)))
    {
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.sensorType), CONST_SENSORTYPE_CubeIoRtd);
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.tempSensor), CONST_TEMPSENSOR_ADS1220);
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.reserved1), 0);
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.reserved2), 0);
        crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t));
    }
}

void eeprom_reset_factory(void)
{
    eeprom_writeint32((uint32_t*) & (eepUserConfig.floatByteOrder), CONST_FLOATBYTEORDER_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved1), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved2), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved3), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved4), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved5), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved6), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved7), 0);
    crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

    eeprom_reset_calibration();
}

void eeprom_reset_calibration(void)
{
}
