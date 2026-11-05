#include "eeprom.h"
#include "utils.h"
#include "sensor.h"
#include "hal_eeprom.h"
#include "sensor_static_data.h"

#define EEPROM_BASE_ADDR 0x08080000

#define EEPROM_ADDR0 (EEPROM_BASE_ADDR+0)
STRUCT_USERCONFIG eepUserConfig __attribute__((at(EEPROM_ADDR0)));

#define EEPROM_ADDR1 (EEPROM_ADDR0+sizeof(STRUCT_USERCONFIG))
STRUCT_SENSORCONFIG eepSensorConfig __attribute__((at(EEPROM_ADDR1)));

#define EEPROM_ADDR2 (EEPROM_ADDR1+sizeof(STRUCT_SENSORCONFIG))
STRUCT_FACTORYCONFIG eepFactoryConfig __attribute__((at(EEPROM_ADDR2)));

#define EEPROM_ADDR3 (EEPROM_ADDR2+sizeof(STRUCT_FACTORYCONFIG))
STRUCT_CHANNEL_CONFIG eepChannelConfig __attribute__((at(EEPROM_ADDR3)));

//DigiEC = DigiEC
const char* CONST_PRODUCT_INFO_CubeIORtd =  "13INFWIN  CubeIO1.0RTD000063000F";

const char* CONST_DEFAULT_SN = "INFWIN  ";

void eeprom_init(void)
{
    if(crc32EepromCheck((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t)))
    {
        for(uint8_t i = 0; i < 2; i++)
        {
            uint16_t hi16 = MAKE_16BIT(CONST_DEFAULT_SN[4 * i], CONST_DEFAULT_SN[4 * i + 1]);
            uint16_t lo16 = MAKE_16BIT(CONST_DEFAULT_SN[4 * i + 2], CONST_DEFAULT_SN[4 * i + 3]);
            uint32_t value = MAKE_32BIT(hi16, lo16);
            eeprom_writeint32(&eepUserConfig.sn[i], value);
        }
        eeprom_writeint32((uint32_t*) & (eepUserConfig.sdi12Address), CONST_SDI12ADDRESS_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.temperatureUnit), CONST_TEMPUNIT_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepUserConfig.temperatureOffset), CONST_TEMPOFFSET_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.filterWindow), CONST_FILTERWINDOW_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.asyncAcqInterval), CONST_ASYNCACQINTERVAL_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved5), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved6), 0);
        eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved7), 0);
        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));
    }

    if(crc32EepromCheck((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t)))
    {
        eeprom_writeint32((uint32_t*) & (eepSensorConfig.tempSource), CONST_TEMPSOURCE_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.tempManualInput), CONST_TEMP_MANUALINPUT_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.ecRefTemperature), CONST_ECREFTEMPERATURE_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepSensorConfig.ecTCompensationMode), CONST_ECTCOMPENSATIONMODE_DEFAULT);
        eeprom_writeint32((uint32_t*) & (eepSensorConfig.ecCalibMode), CONST_ECCALIBMODE_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.cellConstantValue), CONST_CELLCONSTANTVALUE_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefECTCompensation), CONST_COEF_ECTCOMPENSATION_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefTDS), CONST_COEF_TDS_DEFAULT);
        eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefSalinity), CONST_COEF_SALINITY_DEFAULT);
        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));
    }

    if(crc32EepromCheck((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t)))
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            uint16_t hi16 = MAKE_16BIT(CONST_PRODUCT_INFO_CubeIORtd[4 * i], CONST_PRODUCT_INFO_CubeIORtd[4 * i + 1]);
            uint16_t lo16 = MAKE_16BIT(CONST_PRODUCT_INFO_CubeIORtd[4 * i + 2], CONST_PRODUCT_INFO_CubeIORtd[4 * i + 3]);
            uint32_t value = MAKE_32BIT(hi16, lo16);
            eeprom_writeint32(&eepFactoryConfig.productInfo[i], value);
        }
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.sensorType), CONST_SENSORTYPE_CubeIoRtd);
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.tempSensor), CONST_TEMPSENSOR_ADS1220);
        eeprom_writeint32((uint32_t*) & (eepFactoryConfig.cellConstant), CONST_CELLCONSTANT_1P0);
        crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t));
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
}

void eeprom_reset_factory(void)
{
    eeprom_writeint32((uint32_t*) & (eepUserConfig.temperatureUnit), CONST_TEMPUNIT_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepUserConfig.temperatureOffset), CONST_TEMPOFFSET_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.filterWindow), CONST_FILTERWINDOW_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.asyncAcqInterval), CONST_ASYNCACQINTERVAL_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved5), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved6), 0);
    eeprom_writeint32((uint32_t*) & (eepUserConfig.reserved7), 0);
    crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

    eeprom_writeint32((uint32_t*) & (eepSensorConfig.tempSource), CONST_TEMPSOURCE_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.tempManualInput), CONST_TEMP_MANUALINPUT_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.ecRefTemperature), CONST_ECREFTEMPERATURE_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepSensorConfig.ecTCompensationMode), CONST_ECTCOMPENSATIONMODE_DEFAULT);
    eeprom_writeint32((uint32_t*) & (eepSensorConfig.ecCalibMode), CONST_ECCALIBMODE_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.cellConstantValue), CONST_CELLCONSTANTVALUE_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefECTCompensation), CONST_COEF_ECTCOMPENSATION_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefTDS), CONST_COEF_TDS_DEFAULT);
    eeprom_writefloat((uint32_t*) & (eepSensorConfig.coefSalinity), CONST_COEF_SALINITY_DEFAULT);
    crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

    eeprom_reset_calibration();
}

void eeprom_reset_calibration(void)
{
}
