#ifndef LTIME_H_
#define LTIME_H_
#include "l_types.h"

#define CMOS_TIME_ADDR_PORT 0x70
#define CMOS_TIME_DATA_PORT 0x71


#define CMOS_SECOND 0x00
#define CMOS_MINUTE 0x02
#define CMOS_HOUR 0x04
#define CMOS_WDAY 0x06
#define CMOS_MDAY 0x07     
#define CMOS_MONTH 0x08   
#define CMOS_YEAR 0x09 

#define CMOS_STATUS_B 0x0B
#define CMOS_STATUS_B_DATA_MODE 0x04    // 0: BCD 1:BIN
#define CMOS_NMI_BIT 0x80



#define MINUTE_SEC 60                   // 各个单位的秒数
#define HOUR_SEC (MINUTE_SEC * 60)
#define DAY_SEC (HOUR_SEC * 24)
#define YEAR_SEC (DAY_SEC * 365)


/// @brief 每个月已经过去的天数, 第 0 个月 用 0 代替；
///        这里是用闰年算的，所以2月加的是 29
static int month[13] = {
    0,
    0,
    (31),
    (31 + 29),
    (31 + 29 + 31),
    (31 + 29 + 31 + 30),
    (31 + 29 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
};

typedef struct tm 
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;            //  一个月的第几天
    int tm_mon;
    int tm_year;
    int tm_wday;            //  一个星期的第几天
    int tm_yday;            //  一年的第几天
    //int tm_uisdst;
} tm;


typedef uint32_t time_t ;



void time_read(tm *time);

time_t mktime(tm *time);

void time_init();

#endif