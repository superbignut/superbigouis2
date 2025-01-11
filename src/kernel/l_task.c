#include "l_task.h"
#include "l_debug.h"
#include "l_printk.h"
#include "l_types.h"
#include "l_stdlib.h"

#define PAGE_SIZE 0x1000                                //  每个任务分配一页的栈 4G / 4k = 1M
                                                        //  这里 应该是 虚拟机内部的实现上的区别，加上delay后 bochs 打印是正常的，qemu就会不均匀
                                                        //  这里如果是 0x1000 和 0x2000 qemu 总是会出现 AB 打印不均匀的问题， 换成其他地址就ok
task_t *a = (task_t *)0x2000;                           //  这两个地方应该是空的(其实也是 loader之前的地址)
task_t *b = (task_t *)0x3000;                           //  相当于创建了两个上下文-寄存器组

/// 切换到下一个函数
extern void task_switch(task_t *next);

/// @brief 创建一个进程 并在这个页的最高位写入进程信息（寄存器组），并更新页的前4个字节是栈顶的位置
/// @param task 
/// @param target 
static void task_create(task_t *task, target_t target)
{

    uint32_t stack = (uint32_t)task + PAGE_SIZE;        //  栈顶
    
    stack -= sizeof(task_frame_t);                      //  保存进程信息
    task_frame_t *frame = (task_frame_t *)stack;        

    frame->edi = 0x11111111;
    frame->esi = 0x22222222;
    frame->ebx = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void *)target;                        //  这个是最关键的位置，也就是这个进程可执行程序的位置
    
    task->stack = (uint32_t *)stack;                    //  更新栈顶指针
}

/// @brief 返回当前调用的函数
/// @return 
task_t *running_task()
{
    asm volatile(
        "movl %esp, %eax \n"
        "andl $0xfffff000, %eax \n"
        );
}

/// @brief 判断当前的栈顶地址是 1 开头（1xxx）, 还是， 2 开头（2xxx）； 其实，printA之后就是1开头， printB之后就是2开头
void schedule()
{
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    task_switch(next);
                                                        //  汇编中保存的上下文 指的就是这里   1 
}

/// @brief 循环很关键
/// @return 
uint32_t _ofp thread_a()    //  这里开启栈帧感觉问题也不大的
{
    asm volatile("sti");
    while(True)
    {
        printk("A");
        delay(40000);
    }
}


/// @brief 同上
/// @return 
uint32_t _ofp thread_b()
{
    asm volatile("sti");
    while(True)
    {
        printk("B");
        delay(40000);
    }
}

/// @brief 这个初始化，完成了AB任务切换的启动
void task_init()
{
    task_create(a, thread_a);
    task_create(b, thread_b);
    schedule();     //  这里直接调 thread_a 也可以，开中断 + 死循环 也可以
}