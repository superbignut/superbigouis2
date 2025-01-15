#include "l_time.h"
#include "l_io.h"
#include "l_stdlib.h"
#include "l_debug.h"
#include "l_printk.h"

time_t start_time;      // 开机时间

int centry;

/// @brief 读出 addr 寄存器对应的值
/// @param addr 
static uint8_t cmos_read(uint8_t addr)
{
    write_byte_to_port(CMOS_TIME_ADDR_PORT, CMOS_NMI_BIT | addr);
    return read_byte_from_port(CMOS_TIME_DATA_PORT);
}

/// @brief CMOS 是否使用BCD码作为返回
/// @return 
static bool CMOS_RETURN_BCD()
{
    return !(cmos_read(CMOS_STATUS_B) & CMOS_STATUS_B_DATA_MODE);
}

/// @brief 把 bcd 时间读到 time 中
/// @param time 
static void time_read_bcd(tm *time)
{
    do
    {
        time->tm_sec =  cmos_read(CMOS_SECOND);
        time->tm_min =  cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_wday = cmos_read(CMOS_WDAY);
        time->tm_mday = cmos_read(CMOS_MDAY);
        time->tm_mon =  cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);           //  这个返回的是 以1900年为基准的BCD， 超了 100 应该有回零了
    } while (time->tm_sec != cmos_read(CMOS_SECOND));   //  只有相等的时候跳出，即误差在1s之内
    
}

/// @brief 计算这是一年中的第几天 copy from "onix"
/// @param time 除了 yday 已经处理好的 tm 对象
static int get_yday(tm *time)
{
    int ret_sec = month[time->tm_mon];

    ret_sec += time->tm_mday;
    
    int year;
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;
    
                                                //  如果不是闰年，并且 2 月已经过去了，则减去一天

    if (year % 4 && time->tm_mon > 2)           //  闰 且 过了2月
    {
        ret_sec -= 1;
    }
    return ret_sec;                             //  其实是 (year - 70  + 1900) % 4 如果能除开就是闰年
}

/// @brief 把时间读到 time 中
/// @param time 
void time_read(tm *time)
{
    time_read_bcd(time);

    if(CMOS_RETURN_BCD())
    {
        time->tm_sec = bcd_to_bin(time->tm_sec);
        time->tm_min = bcd_to_bin(time->tm_min);
        time->tm_hour = bcd_to_bin(time->tm_hour);
        time->tm_wday = bcd_to_bin(time->tm_wday);
        time->tm_mday = bcd_to_bin(time->tm_mday);
        time->tm_mon = bcd_to_bin(time->tm_mon);
        time->tm_year = bcd_to_bin(time->tm_year);
        time->tm_yday = get_yday(time);
    }
}

/// @brief 返回从 1970 开始的秒数， copy from onix
/// @param time 
/// @return 
time_t mktime(tm *time)
{
    time_t res;
    // Todo 有点混乱，先不实现
    /*     int year;                            // 1970 年开始的年数， 这里对应 python 的time 时间戳返回的就是 从1970 开始的时间
    if (time->tm_year >= 70)                    // 下面从 1900 年开始的年数计算
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;        //  2025： 25 < 70 所以要加 100 即  100 + 25 - 70 = 55年
    res = YEAR_SEC * year;                      // 这些年经过的秒数时间
                                            
    res += DAY_SEC * ((year + 1) / 4);          // 已经过去的闰年，每个加 1 天
                                            
    res += month[time->tm_mon] * DAY_SEC;       // 已经过完的月份的时间
                                            
    if (time->tm_mon > 2 && ((year + 2) % 4))   // 如果 2 月已经过了，并且当前不是闰年，那么减去一天
        res -= DAY_SEC;
                                            
    res += DAY_SEC * (time->tm_mday - 1);       // 这个月已经过去的天
                                            
    res += HOUR_SEC * time->tm_hour;            // 今天过去的小时
                                            
    res += MINUTE_SEC * time->tm_min;           // 这个小时过去的分钟                                        
    res += time->tm_sec; */                     // 这个分钟过去的秒

    return res;
}

/// @brief 时钟初始化：打印当前时间
void time_init(){
    tm time;

    time_read(&time);
    printk("#### CMOS TIME INIT...\n");
    
    /* DEBUGK("timestamp: 20%d-%02d-%02d %02d:%02d:%02d\n", 
            time.tm_year, time.tm_mon, time.tm_mday,
            time.tm_hour, time.tm_min, time.tm_sec); */
}