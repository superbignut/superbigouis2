#include "l_rtc.h"
#include "l_io.h"
#include "l_stdlib.h"
#include "l_interrupt.h"
#include "l_debug.h"
#include "l_assert.h"
#include "l_printk.h"

/// @brief rtc 读端口
/// @param addr 
/// @return 
static uint8_t rtc_read(uint8_t addr)
{
    write_byte_to_port(CMOS_TIME_ADDR_PORT, CMOS_NMI_BIT | addr);
    return read_byte_from_port(CMOS_TIME_DATA_PORT);
}

/// @brief rtc 写端口
/// @param addr 
/// @param data 
static void rtc_write(uint8_t addr, uint8_t data)
{
    write_byte_to_port(CMOS_TIME_ADDR_PORT, CMOS_NMI_BIT | addr);
    write_byte_to_port(CMOS_TIME_DATA_PORT, data);
}

/// @brief 清空 C 寄存器的标志位
static void rtc_interrupt_flag_clear()
{
    rtc_read(CMOS_REGC);
}

/// @brief rtc 中断处理函数
/// @param vector 
static void rtc_handler(int vector)
{
    assert(vector == 0x28);
    set_alarm_sec(2);
    rtc_interrupt_flag_clear();         //  这里感觉应该有一个先后顺序的...
    send_eoi(vector);
    DEBUGK("INT BY RTC...%d\n", vector);
}

/// @brief 设置 sec 秒后的闹钟中断
/// @param sec 
void set_alarm_sec(uint32_t sec)
{
    tm time;   
    time_read(&time);                   //  先找到当下时间

    time.tm_sec += sec % 60;            //  找到 sec秒之后的时间
    sec /= 60;
    time.tm_min += sec % 60;
    sec /= 60;
    time.tm_hour += sec;

    if(time.tm_sec >= 60)               //  进位更新
    {
        time.tm_sec -= 60;
        time.tm_min += 1;
    }

    if(time.tm_min >= 60)
    {
        time.tm_min -= 60;
        time.tm_hour += 1;
    }

    if(time.tm_hour >= 24)
    {
        time.tm_hour -= 24;
    }

    rtc_write(REGA_SECOND_ALARM_BIT, bin_to_bcd(time.tm_sec));      //  设置闹钟 bcd
    rtc_write(REGA_MINUTE_ALARM_BIT, bin_to_bcd(time.tm_min));
    rtc_write(REGA_HOUR_ALARM_BIT, bin_to_bcd(time.tm_hour));
}

/// @brief rtc 初始化， 打开闹钟中断
void rtc_init()
{
    rtc_write(CMOS_REGB, 0b00100010);               //  闹钟中断 | 24 小时制
    // rtc_write(CMOS_REGA, (rtc_read(CMOS_REGA) & 0x80) | 0b00100000 | 0b00001111);       //  最高位不变 32.768MHZ 2HZ

    set_alarm_sec(2);                               //  设定若干秒后 发生 闹钟中断 

    set_hardware_interrupt_handler(RTC_IRQ, rtc_handler);
    set_hardware_interrupt_mask(RTC_IRQ, True);
    set_hardware_interrupt_mask(CASCADE_IRQ, True); // 这里因为 rtc 的中断在从片上了，需要打开从片

    printk("#### RTC INIT...\n");
}