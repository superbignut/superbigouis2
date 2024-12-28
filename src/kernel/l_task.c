#include "l_task.h"
#include "l_debug.h"
#include "l_printk.h"

#define PAGE_SIZE 0x1000                                //  每个任务分配一页的栈 4G / 4k = 1M

task_t *a = (task_t *)0x1000;                           //  这两个地方应该是空的(其实也是 loader之前的地址)
task_t *b = (task_t *)0x2000;                           //  相当于创建了两个上下文-寄存器组

/// 切换到下一个函数
extern void task_switch(task_t *next);

/// @brief 创建一个进程 并在这个页的最高位写入进程信息（寄存器组），并更新栈顶指针
/// @param task 
/// @param target 
static void task_create(task_t *task, target_t target){

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
task_t *running_task(){
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n"
        );
}

void schedule(){
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    task_switch(next);
                                                        //  汇编中保存的上下文 指的就是这里   1 
}

uint32_t thread_a(){
    while(True){
        printk("A");
        schedule();
                                                        //  汇编中保存的上下文，1 执行完之后 回到 2 
    }
}


uint32_t thread_b(){
    while(True){
        printk("B");
        schedule();
    }
}


void task_init(){
    task_create(a, thread_a);
    task_create(b, thread_b);
    schedule();
}