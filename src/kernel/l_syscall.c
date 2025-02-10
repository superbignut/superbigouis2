#include "l_syscall.h"
#include "l_assert.h"
#include "l_printk.h"
#include "l_task.h"

///  系统调用处理函数 数组
handler_t syscall_table[SYSCALL_SIZE];

/// @brief 
/// @param num 
void syscall_check(uint32_t num)
{
    if(num >= SYSCALL_SIZE)
    {
        panic("system call num error.");
    }
}

/// @brief 默认系统调用函数
static void syscall_default_handler()
{
    panic("currrent system call not imply...");
}

/// @brief 
/// @param num 
/// @return 
static inline uint32_t _syscall0(uint32_t num)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        :"=a"(ret)
        :"a"(num));
    return ret;
}

/// @brief 0号 系统调用函数处理函数
/// @return 
uint32_t syscall_test_handler()
{
    return 10;                          //  测试返回值， _syscall_handler 会继续把 eax 返回， 在 int 0x80 之后可以在eax 中查看
}

/// @brief 1号 系统调用处理函数， 只有一个任务时调度会 报错
void syscall_yield_handler()
{
    //  yield 系统调用会让出 当前 cpu 占用 去调度其他的线程
    schedule();
}

/// @brief 0号系统调用
/// @return 
uint32_t syscall_test()
{
    return _syscall0(SYS_CALL_TEST);
}

/// @brief 1号系统调用
void syscall_yield()
{
    _syscall0(SYS_CALL_YIELD);
}


/// @brief 系统调用初始化， 初始化系统调用数组 syscall_table
void syscall_init()
{
    for(size_t i = 0; i < SYSCALL_SIZE; ++i)
    {
        syscall_table[i] = syscall_default_handler;
    }

    syscall_table[0] = syscall_test_handler;
    syscall_table[1] = syscall_yield_handler;
    printk("#### SYSTEM CALL INIT...\n");
}


