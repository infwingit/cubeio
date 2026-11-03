#include "main.h"
#include "utils.h"
#include "globals.h"
#include "eeprom.h"
#include "usart.h"
#include "sensor.h"
#include "modbus_reg.h"

MEASURE_VALUE g_MeasureValue;
DEBUG_STATUS g_debugStatus = DEBUG_DISABLE;

void resetMeasValue(void)
{
}

uint8_t GetDebugEnabled()
{
    return g_debugStatus;
}

void SetDebugEnabled(DEBUG_STATUS value)
{
    g_debugStatus = value;
}

void globalVarInit()
{
    g_MValue.SensorStatus = SENSOR_STATUS_OK;
    g_debugStatus = DEBUG_DISABLE;
}
