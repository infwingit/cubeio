#include "main.h"
#include "utils.h"
#include "globals.h"
#include "eeprom.h"
#include "sensor.h"

DEBUG_STATUS g_debugStatus = DEBUG_DISABLE;
DEBUG_VAR g_DebugVar;

void resetMeasValue(void)
{
}

void setDebugEnabled(void)
{
    g_debugStatus = DEBUG_ENABLE;
}

DEBUG_STATUS getDebugEnabled(void)
{
    return g_debugStatus;
}

void globalVarInit()
{
    g_debugStatus = DEBUG_DISABLE;
    g_MValue.SensorStatus = SENSOR_STATUS_OK;
    g_debugStatus = DEBUG_DISABLE;
}
