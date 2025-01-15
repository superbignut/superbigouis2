#ifndef LRTC_H_
#define LRTC_H_
#include "l_types.h"
#include "l_time.h"                     //  rtc 使用的端口 也是 0x70



#define REGA_SECOND_ALARM_BIT 0x01      //  寄存器 A 中的各个闹钟位
#define REGA_MINUTE_ALARM_BIT 0x03
#define REGA_HOUR_ALARM_BIT 0x05

#define CMOS_REGA 0x0A                  //  三个寄存器位置
#define CMOS_REGB 0x0B
#define CMOS_REGC 0x0C      


void set_alarm_sec(uint32_t sec);

void rtc_init();

#endif