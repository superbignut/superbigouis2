#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"
#include "l_stdargs.h"
#include "l_printk.h"
#include "l_assert.h"
#include "l_debug.h"
#include "l_gdt.h"
#include "l_task.h"
#include "l_interrupt.h"
#include "l_stdlib.h"
#include "l_clock.h"
#include "l_time.h"
#include "l_rtc.h"


/*
    内核初始化

    
*/
void kernel_init()
{
    console_init();         //  光标、打印初始化

    gdt_init();             //  gdt 全局描述符表初始化

    interrupt_init();       //  pic、idt 初始化

    clock_init();           //  8253 时钟、beep 初始化

    time_init();            //  CMOS 时间初始化

    rtc_init();             //  rtc 实时时钟初始化

    asm volatile("sti");    //  开中断

    while(True);            //  死循环
}