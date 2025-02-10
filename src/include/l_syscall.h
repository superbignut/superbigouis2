#ifndef LSYSCALL_H_
#define LSYSCALL_H_

#include "l_interrupt.h"


///  系统调用数量
#define SYSCALL_SIZE 64


/// @brief 系统调用 编号枚举
typedef enum syscall_t
{
    SYS_CALL_TEST,
    SYS_CALL_YIELD,
} syscall_t;


void syscall_init();

// uint32_t syscall_test_handler();

// void syscall_yield_handler();


uint32_t syscall_test();

void syscall_yield();

#endif