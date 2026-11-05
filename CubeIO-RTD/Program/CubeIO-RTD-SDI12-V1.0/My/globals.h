#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "main.h"

typedef struct DEBUG_VAR
{
    uint16_t wakeupCounter;
    uint16_t idleCounter;
    uint16_t tcCounter;
    uint16_t txDoneCounter;
    uint16_t rxDoneCounter;
    uint16_t uartFrameErrorCounter;
    uint16_t rxErrorCounter;
    uint16_t standbyCounter;
} DEBUG_VAR;
extern DEBUG_VAR g_DebugVar;

typedef enum DEBUG_STATUS
{
    DEBUG_DISABLE = 0,
    DEBUG_ENABLE = 1
} DEBUG_STATUS;
extern DEBUG_STATUS g_debugStatus;

extern void globalVarInit(void);
extern void resetMeasValue(void);

extern void setDebugEnabled(void);
extern DEBUG_STATUS getDebugEnabled(void);

#endif
