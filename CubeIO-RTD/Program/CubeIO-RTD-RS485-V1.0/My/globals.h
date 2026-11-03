#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "main.h"
#include "modbus_reg.h"

typedef enum DEBUG_STATUS
{
    DEBUG_DISABLE = 0,
    DEBUG_ENABLE = 1
} DEBUG_STATUS;
extern DEBUG_STATUS g_debugStatus;

extern void globalVarInit(void);
extern void resetMeasValue(void);

extern uint8_t GetDebugEnabled(void);
extern void SetDebugEnabled(DEBUG_STATUS value);
extern void globalVarInit(void);

#endif
