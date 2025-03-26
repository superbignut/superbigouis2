/**
 * @file main.c 
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
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
#include "l_memory.h"
#include "l_bitmap.h"
#include "l_syscall.h"

/**
 * @brief This is the first C-function which was jumped from start.asm after going to protected mode of i386-cpu.
 *        Therefore, at the begining of this function, some critical/important initialization function were called
 *        as below. The initialization process include the Physics-Memory-Init, Paging-Init, Clock-Chip init and so
 *        on. 
 *        There are also some Initialization called that were commented out, for the reason of moving into start.asm.
 *        But I can't remember... // Todo
 * @param 
 * 
 * @author
 * 
 * @return
 */
void kernel_init()
{
    // console_init();      //  光标、打印初始化

    // gdt_init();          //  gdt 全局描述符表初始化

    memory_map_init();      //  内存页初始化

    paging_init();          //  分页初始化

    interrupt_init();       //  pic、idt 初始化

    clock_init();           //  8253 时钟、beep 初始化

    // time_init();            //  CMOS 时间初始化

    // rtc_init();             //  rtc 实时时钟初始化

    // asm volatile("sti");    //  开中断

    // extern void task_init();

    task_init();                //  多线程初始化

    syscall_init();             //  系统调用初始化

    set_if_flag(True);          

    while(True);                //  死循环
}