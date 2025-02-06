#include "l_task.h"
#include "l_debug.h"
#include "l_printk.h"
#include "l_types.h"
#include "l_stdlib.h"
#include "l_bitmap.h"
#include "l_os.h"
#include "l_memory.h"
#include "l_assert.h"
#include "l_interrupt.h"

#define PAGE_SIZE 0x1000                                //  每个任务分配一页的栈 4G / 4k = 1M
                                                        //  这里 应该是 虚拟机内部的实现上的区别，加上delay后 bochs 打印是正常的，qemu就会不均匀
                                                        //  这里如果是 0x1000 和 0x2000 qemu 总是会出现 AB 打印不均匀的问题， 换成其他地址就ok
task_t *a_deprecated = (task_t *)0x2000;                           //  这两个地方应该是空的(其实也是 loader之前的地址)
task_t *b_deprecated = (task_t *)0x3000;                           //  相当于创建了两个上下文-寄存器组

/// 切换到下一个函数
extern void task_switch_deprecated(task_t *next);

/// @brief 其实只有参数类型不一样， 适配 task_t_new 的 切换函数
/// @param next 
extern void task_switch(task_t_new *next);

/// 虚拟内存位图
extern bitmap_t kernel_bitmap;

/// 最多的任务数
#define NUM_TASKS 64

/// @brief 任务数组
static task_t_new *task_table[NUM_TASKS];

/// @brief 找到正在运行的进程的 页 的首地址
/// @return 
task_t_new *running_task()
{
    asm volatile(
        "movl %esp, %eax \n"
        "andl $0xfffff000, %eax \n"
        );
}


/// @brief 从 task_table 中分配一个 空闲任务，也即使用 alloc_kernel_page 分配一个空闲页出来， 并放入 task_table
/// @return 
static task_t_new *get_free_task()
{
    for(int i = 0; i < NUM_TASKS; ++i)
    {
        if(task_table[i] == NULL)
        {
            task_table[i] = (task_t_new *)alloc_kernel_page(1);     //  返回空闲页首地址
            
            return task_table[i];
        }
    }
    panic("no more free task!");
}

/// @brief 从任务数组中 查找某种状态的任务
/// @param state 
/// @return 
static task_t_new *task_search_in_table(task_state_t state)
{
    assert(get_if_flag() == 0);                     //  下面的操作不允许中断
    task_t_new *task = NULL;    
    task_t_new *current = running_task();       //  
    task_t_new *ptr = NULL;

    for(int i = 0; i < NUM_TASKS; ++i)
    {
        ptr = task_table[i];

        if(ptr == NULL)
            continue;
        if(ptr->state != state)
            continue;
        if(ptr == current)
            continue;
        //  第一次找到合适的 || 时间片更少（优先级更高） || 上次启动时间更早（防止饿死: 体现在 ABC切换到B时，B会切换到C而不是A)
        if(task == NULL || task->ticks < ptr->ticks || ptr->jiffies < task->jiffies)
            task = ptr;
    }
    return task;
}


/// @brief 任务表 初始化， 任务数组清空
static void task_table_setup()
{
    task_t_new *task = running_task();
    task->magic = OS_MAGIC;
    task->ticks = 1;

    memory_set(task_table, 0, sizeof(task_table));  // 清空
}

/// @brief 找到进程状态是 TASK_READY 的进程， 并使用task_switch 进行切换
void schedule()
{
    task_t_new *current = running_task();
    task_t_new *next = task_search_in_table(TASK_READY);

    assert(next->magic == OS_MAGIC);
    assert(next != NULL);

    if(current->state == TASK_RUNNING)          //  更新两个任务的状态
    {
        current->state = TASK_READY;
    }
    next->state = TASK_RUNNING;

    task_switch(next);
}

/// @brief 创建内核进程(分配一个空闲任务页)
/// @param target 
/// @param name 
/// @param priority 
/// @param uid 
static task_t_new *task_create(target_t target, const char *name, uint32_t priority, uint32_t uid)
{
    task_t_new *task = get_free_task();                 //  分配一个空闲任务页
    memory_set(task, 0, PAGE_SIZE);

    uint32_t stack = (uint32_t)task + PAGE_SIZE;        //  栈顶指针 task=0x102000 对应 stack = 0x103000

    stack -= sizeof(task_frame_t);                      //  栈顶减少一个 task_frame_t 的空间

    task_frame_t *frame = (task_frame_t *)stack;        //  将信息存到 预留的 task_frame_t 中
    frame->edi = 0x11111111;
    frame->esi = 0x22222222;
    frame->ebx = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void *)target;                        //  设置任务进程函数

    string_copy((char *)task->name, name);              //  进程名字

    task->stack = (uint32_t *)stack;                    //  栈
    task->priority = priority;                          //  优先级
    task->ticks = priority;                             //  剩余时间片 = 优先级
    task->uid = uid;
    task->jiffies = 0;                                  //  最初的 jiffies 设置为 1
    task->state = TASK_READY;                           //  就绪
    task->vmap = &kernel_bitmap;                        //  暂未使用
    task->pde = KERNEL_PAGE_DIR;
    task->magic = OS_MAGIC;

    return task;
}


/// @brief 创建一个进程 并在这个页的最高位写入进程信息（寄存器组），并更新页的前4个字节是栈顶的位置
/// @param task 
/// @param target 
static void task_create_deprecated(task_t *task, target_t target)
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

/// @brief 返回当前调用的函数的 页首地址
/// @return 
task_t *running_task_deprecated()
{
    asm volatile(
        "movl %esp, %eax \n"
        "andl $0xfffff000, %eax \n"
        );
}

/// @brief 判断当前的栈顶地址是 1 开头（1xxx）, 还是， 2 开头（2xxx）； 其实，printA之后就是1开头， printB之后就是2开头
void schedule_deprecated()
{
    task_t *current = running_task_deprecated();
    task_t *next = current == a_deprecated ? b_deprecated : a_deprecated;
    task_switch_deprecated(next);
                                                        //  汇编中保存的上下文 指的就是这里   1 
}

/// @brief 循环很关键
/// @return 
uint32_t _ofp thread_a_deprecated()    //  这里开启栈帧感觉问题也不大的
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
uint32_t _ofp thread_b_deprecated()
{
    asm volatile("sti");
    while(True)
    {
        printk("B");
        delay(40000);
    }
}


uint32_t _ofp thread_a()    //  这里开启栈帧感觉问题也不大的
{
    // asm volatile("sti");
    set_if_flag(True);
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
    // asm volatile("sti");
    set_if_flag(True);
    while(True)
    {
        printk("B");
        delay(40000);
    }
}

/// @brief 同上
/// @return 
uint32_t _ofp thread_c()
{
    // asm volatile("sti");
    set_if_flag(True);
    while(True)
    {
        printk("C");
        delay(40000);
    }
}

/// @brief 这个初始化，完成了AB任务切换的启动
void task_init_deprecated()
{
    task_create_deprecated(a_deprecated, thread_a_deprecated);
    task_create_deprecated(b_deprecated, thread_b_deprecated);
    schedule_deprecated();     //  这里直接调 thread_a 也可以，开中断 + 死循环 也可以
}

/// @brief 任务初始化 ABC 轮流打印， 使用 时钟中断 来进行调度，每个调度周期 由分配的时间片决定
void task_init()    
{
    task_table_setup();         //  初始化 任务数组

    task_create(thread_a, "a", 5, KERNEL_USER);
    task_create(thread_b, "b", 5, KERNEL_USER);
    task_create(thread_c, "c", 5, KERNEL_USER);
}