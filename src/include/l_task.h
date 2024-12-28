#ifndef LTASK_H_
#define LTASK_H_

#include "l_types.h"

typedef uint32_t target_t();                //  第一次见， 这是一个函数类型， 但一般都是要和 * 一起来表示 target_t* 一个函数指针


/// @brief 进程信息
typedef struct task_t{

    uint32_t *stack;                        //   页的前四个字节是这个程序的栈的地址

} task_t;

/// @brief ABI寄存器
typedef struct task_frame_t
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    void (*eip)();
} task_frame_t;


void task_init();


#endif