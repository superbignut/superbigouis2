#ifndef LTASK_H_
#define LTASK_H_

#include "l_types.h"
#include "l_bitmap.h"

#define KERNEL_USER 0       //  内核用户
#define NORMAL_USER 1       //  普通用户

#define TASK_NAME_LEN   16  //  PCB 中 name 长度

typedef uint32_t (*target_t)();                 //  第一次见， 这是一个函数类型， 但一般都是要和 * 一起来表示 target_t* 一个函数指针
                                                //  改成函数指针更好理解
/// @brief 进程信息，在进程上下文中，和中断上下文中使用，只使用了 栈顶一个变量，后续使用 task_t_new 作为更丰富的进程 pcb 
typedef struct task_t
{

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

/// @brief 进程的状态信息
typedef enum task_state_t
{
    TASK_INIT,          //  初始化
    TASK_RUNNING,       //  运行
    TASK_READY,         //  就绪
    TASK_BLOCKED,       //  阻塞
    TASK_SLEEPING,      //  睡眠
    TASK_WAITING,       //  等待
    TASK_DIED           //  死亡
} task_state_t;

/// @brief 进程 pcb 信息
typedef struct task_t_new
{
    uint32_t       *stack;                  //  内核栈此时的栈顶
    task_state_t    state;                  //  任务状态
    uint32_t        priority;               //  任务优先级
    uint32_t        ticks;                  //  剩余时间片
    uint32_t        jiffies;                //  上次执行时的全局时间片
    char            name[TASK_NAME_LEN];    //  任务名
    uint32_t        uid;                    //  用户 id
    uint32_t        pde;                    //  页目录 物理地址
    bitmap_t       *vmap;                   //  进程虚拟内存位图
    uint32_t        magic;                  //  校验 魔数
} task_t_new;

void task_init();

task_t_new *running_task();

void schedule();

#endif