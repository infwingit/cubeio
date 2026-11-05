#include "main.h"

#ifndef UTILS_DATETIME_H_
#define UTILS_DATETIME_H_

typedef struct my_tm
{
    uint32_t tm_sec;   // seconds after the minute - [0, 60] including leap second
    uint32_t tm_min;   // minutes after the hour - [0, 59]
    uint32_t tm_hour;  // hours since midnight - [0, 23]
    uint32_t tm_mday;  // day of the month - [1, 31]
    uint32_t tm_mon;   // months since January - [0, 11]
    uint32_t tm_year;  // 这里的时间为真实年份
} my_tm;

extern uint32_t kernel_mktime(my_tm* tm);
extern uint32_t GetTimeStamp(void);

#endif /* UTILS_DATETIME_H_ */
