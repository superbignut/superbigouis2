#include "l_clock.h"
#include "l_interrupt.h"
#include "l_io.h"
#include "l_debug.h"
#include "l_printk.h"
#include "l_task.h"
#include "l_assert.h"
#include "l_os.h"


/// @brief 全局时间片
uint32_t jiffies = 0;

/// @brief 时钟中断处理函数，进行进程切换
/// @param vector 
static void clock_handler(int vector)
{
    // DEBUGK("This is %d time to print by int %d.\n", jiffies, vector);
    send_eoi(vector);                           //  不要忘记eoi, 因为 还有 if 位，所以 先 send_eoi 没有问题
    jiffies++;                                  //  全局时间片++ 

    task_t_new *task = running_task();
    assert(task->magic == OS_MAGIC);

    task->jiffies = jiffies;                    //  更新全局 jiffies
    task->ticks--;                              //  时间片 -- 
    
    if(task->ticks == 0)                        //  时间片耗尽, 进行调度， 否则继续执行
    {
        // task->ticks = task->priority;        //  重新赋予 时间片， 可以移动到调度中判断
        schedule();
    }
}


/// @brief 8253 初始化：频率生成器初始化
static void pit_8253_init()
{
    //  这里找机会注释一下 
    //  todo
    write_byte_to_port(PIT_8253_MODE_CMD_REG, 0b00110100);      //  67位：00: channel_0;  
                                                                //  45位：11:low_bit_high_bit;  
                                                                //  123位：010: rate_generate;  
                                                                //  0位：0: 16-bit
                                                                //  写命令寄存器， 将8253注册为频率发生器
    
    write_byte_to_port(PIT_8253_CHANNEL_0, CLOCK_COUNTER & 0xff);       //  注册频率
    write_byte_to_port(PIT_8253_CHANNEL_0, (CLOCK_COUNTER >> 8) & 0xff);      
}


/// @brief 
static void pit_init()
{
    pit_8253_init();                                                    //  pit 初始化
}

/// @brief 通道2 初始化
static void beep_init()
{
    write_byte_to_port(PIT_8253_MODE_CMD_REG, 0b10110110);      // 10: channel_2;  11:low_bit_high_bit;  010: square wave generator;  0: 16-bit
    write_byte_to_port(PIT_8253_CHANNEL_2, BEEP_COUNTER & 0xff);      
    write_byte_to_port(PIT_8253_CHANNEL_2, (BEEP_COUNTER >> 8) & 0xff);   
}

/// @brief 时钟中断初始化，设置处理函数、设置中断屏蔽字
void clock_init()
{
    beep_init();                                                        //  增加了bee...的初始化

    pit_init();                                                         //  8253 初始化
    set_hardware_interrupt_handler(CLOCK_IRQ, clock_handler);           //  设置中断处理函数
    set_hardware_interrupt_mask(CLOCK_IRQ, True);                       //  打开中断屏蔽字
    printk("#### CLOCK AND BEEP INIT...\n");
}

/// @brief 开始 bee...
void start_beep()
{
    uint8_t _tmp;

    _tmp = read_byte_from_port(BEEP_PORT);    
    if(_tmp != (_tmp | 0x03))               //  参考osdev ，0x03具体含义不清楚
    {
        write_byte_to_port(BEEP_PORT, _tmp | 0x03);
    }
}

/// @brief 取消 bee...
void stop_beep()
{
    write_byte_to_port(BEEP_PORT, read_byte_from_port(BEEP_PORT) & 0xfc);
}