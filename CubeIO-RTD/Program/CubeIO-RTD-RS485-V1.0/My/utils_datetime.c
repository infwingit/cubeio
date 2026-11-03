/*
 * utils.c
 *
 *  Created on: 2010-4-30
 *      Author: Administrator
 */
#include "utils_datetime.h"
#include "main.h"

//https://www.cnblogs.com/wangyonghui/archive/2010/07/02/1770227.html
//linux内核计算从1970年1月1日0时起到开机当日经过的秒数，采用的方法不是调用开发环境库中的函数，
//而是linus专门实现的一个。个人觉得在处理闰年问题时很经典，特此贴出来为日后留个纪念。

//时间戳相关知识介绍
//定义：时间戳是指格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数。

//时间戳转换工具
//http://shijianchuo.wiicha.com/
//http://www.cshijian.com/unix-timestamp/
//https://unixtime.org/
//https://www.timestamp-converter.com/

#define MINUTE  60
#define HOUR    (60*MINUTE)
#define DAY     (24*HOUR)
#define YEAR    (365*DAY)

/* interestingly, we assume leap-years */
static int month[12] =
{
    0,
    DAY* (31),
    DAY* (31 + 29),
    DAY* (31 + 29 + 31),
    DAY* (31 + 29 + 31 + 30),
    DAY* (31 + 29 + 31 + 30 + 31),
    DAY* (31 + 29 + 31 + 30 + 31 + 30),
    DAY* (31 + 29 + 31 + 30 + 31 + 30 + 31),
    DAY* (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    DAY* (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    DAY* (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    DAY* (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
};

//本事件戳算法已经在https://unixtime.org/验证
//注意：tm结构体中的year为真实年份如2024，month为0-11
//注解：1.(year+1)/4被称作一个魔幻值，用来计算1970年以来的闰年数，而(year+2)%4则是用来判断是不是闰年.
uint32_t kernel_mktime(my_tm* tm)
{
    uint32_t res;
    uint32_t year;

    year = tm->tm_year - 1970;//从1970年開始计时。year得到的是和1970年的年差
    /* magic offsets (y+1) needed to get leapyears right.*/
    res = YEAR * year + DAY * ((year + 1) / 4);//闰年多一天
    res += month[tm->tm_mon];
    /* and (y+2) here. If it wasn't a leap-year, we have to adjust */
    if(tm->tm_mon > 1 && ((year + 2) % 4))//由于是从1970年算起的，这里year+2就能耦合上闰年。。。换而言之，1972年是闰年，可是年差是2。这里补上2就对齐了。
        res -= DAY;//year不是闰年。则减一天
    res += DAY * (tm->tm_mday - 1);
    res += HOUR * tm->tm_hour;
    res += MINUTE * tm->tm_min;
    res += tm->tm_sec;
    return res;
}

//本事件戳算法已经在https://unixtime.org/验证
uint32_t GetTimeStamp(void)
{
    my_tm rtctime;
    uint16_t year, month, day, hour, minute, second;

    //注意：STM32L0中RTC的year为从2000年开始的时间，month为1-12
    year = 2000 + __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
    month = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    day = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    hour = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
    minute = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    second = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));

    rtctime.tm_year = year;
    rtctime.tm_mon = month - 1;
    rtctime.tm_mday = day;
    rtctime.tm_hour = hour;
    rtctime.tm_min = minute;
    rtctime.tm_sec = second;

    return kernel_mktime(&rtctime);
}


