#include "l_gate.h"
#include "l_assert.h"
#include "l_printk.h"

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
static void syscall_default()
{
    panic("currrent system call not imply...");
}

/// @brief 0号 系统调用函数
/// @return 
static uint32_t syscall_test()
{
    printk("#### system call test...\n");
    return 10;                          //  测试返回值， _syscall_handler 会继续把 eax 返回， 在 int 0x80 之后可以在eax 中查看
}

/// @brief 系统调用初始化， 指定系统调用函数
void syscall_init()
{
    for(size_t i = 0; i < SYSCALL_SIZE; ++i)
    {
        syscall_table[i] = syscall_default;
    }

    syscall_table[0] = syscall_test;
}
