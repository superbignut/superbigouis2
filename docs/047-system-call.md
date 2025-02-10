共用页目录 就是同一个进程 ？


+ 系统调用是用户进程与内核沟通的方式。可以将 CPU 从用户态转向内核态；

+ 这里就是使用 int 0x80 中断 来进行从 用户态 向 系统态的切换
    + eax 用于保存 系统调用编号
    + ebx
    + ecx
    + edx
    + 暂时使用四个参数

    这里要 将 0x80 和系统调用编号区分出来， 0x80就是被使用的一个中断号， 不论哪一个系统调用都是使用这个中断号，但是系统调用号 则各有不同
    体现在 eax 中， 也就是 int 0x80 前 push 的参数

        mov eax, 0
        int 0x80


> Interrupts generated in software with the INT n instruction cannot be masked by the IF flag in the EFLAGS register.

一直都不知道 原来 if 位不能影响 int 指令造成的软件中断


```cpp

_syscall_handler:

    push eax                ;  暂存 eax, 进行参数检查

    call _syscall_check     ;  也就是检查 系统调用号

    pop eax                 ;  系统调用号存在 eax 寄存器中， 后续根据 eax 中不同调用号调用不同的 处理函数

    push 0x2222_2222        ;  类比异常处理中的 错误码
    
    push 0x80               ;  类比异常编号

    push ds
    push es 
    push fs 
    push gs

    pusha                                   ;  EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    push 0x80               ;  参数 4

    push edx                ;  参数 3        

    push ecx                ;  参数 2

    push ebx                ;  参数 1

    call [_syscall_table + eax * 4]         ;  调用 eax 指定的 系统调用 ， 其余的 三个 ebx ecx edx 作为 参数

    add esp, 12                             ;  三个参数的 栈的恢复， 这里之所以 不是 add esp, 16 是为了留一个 参数4， 进而兼容 interrupt_exit
    
    mov dword [esp + 8 * 4], eax            ;  把 eax 放回到 pusha 中的 eax, 进而在 popa 的时候，恢复到 eax 
                                            ;  从而作为 系统调用 的返回值

    jmp interrupt_exit                      ; 这里沿用 异常的 部分, 如下 interrupt_exit 所示


```


```cpp

interrupt_exit:

    pop eax                                 ; 

    popa

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8                              ; 弹出两个参数 编号和错误码

    iret                                    ; 中断结束 恢复 eflag


```


#### 当返回值是一个结构体， 编译是怎么样的

```cpp

typedef struct aaaa
{
    int a;
    int b;
    int c;
} aaa;

aaa __attribute__((optimize("omit-frame-pointer"))) func()
{
    aaa a1;
    a1.a = 1;
    a1.b = 2;
    a1.c = 3;
    return a1;
}
int main()
{
    func(1,2,3);
}

```

#### 编译结果



```cpp
	.file	"test.c"
	.text
	.globl	_func
	.def	_func;	.scl	2;	.type	32;	.endef
_func:
	subl	$12, %esp
	movl	$1, (%esp)
	movl	$2, 4(%esp)
	movl	$3, 8(%esp)
	movl	16(%esp), %eax
	movl	(%esp), %edx
	movl	%edx, (%eax)                    ;  3-> 可以发现 返回值 被放在了这个参数的地址中
	movl	4(%esp), %edx
	movl	%edx, 4(%eax)
	movl	8(%esp), %edx
	movl	%edx, 8(%eax)
	movl	16(%esp), %eax
	addl	$12, %esp
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	call	___main
	leal	16(%esp), %eax          ; 1-> 可以发现 一个 地址参数 存入 eax
	movl	$3, 12(%esp)        
	movl	$2, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)            ; 2-> 这个地址参数 进而传给了 函数 func
	call	_func
	movl	$0, %eax
	leave
	ret
```

因此我们可以看到，如果返回值 是 32 位， 当然只用 eax 就可以了 ，但如果 返回值 体积很大， 就需要 传入一个 地址参数，让返回值 最后能保存到地址中




#### yield 系统调用



这一节 是基于之前的将 schedule 放到 clock 的处理函数中发现的：

```cpp
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
```

这里相当于每个进程有一个时间片， 然后 对于 print a b c 三个 函数来说，出现的现象就是  一会打印出 一堆 A 一会打印出 一堆B ， 一会打印 一堆 C

但是这一节 的 yield 系统调用的目的就是，在我的时间片还没有 用完的时候，我就主动让出 cpu， 其实也就是主动的 schedule， 但是这里的实现上是 将yield 包在了 int 0x80-1 的

系统调用中（软件中断）, 因此，只有 时钟中断和 系统调用 两种情况会 触发 schedule， 进而作者在 shcedule 中 加了一个 不可中断的 IF 位 检测也就是合理的：

因为，如果我没理解错的话： "int 触发系统调用时， cpu 会将 eflags 入栈 并 清空 if"

```cpp
void schedule()
    assert(!get_interrupt_state()); // 不可中断

```


总结一下就是，挺套娃的，加了系统调用之后，任务切换的层数越来越多, 以 A 切 B 再切 A 举例：

```cpp
uint32_t _ofp thread_a()    
{
    set_if_flag(True);
    while(True)
    {
        printk("A");
        syscall_yield();                        //  1 这里进行切换，进入 syscall_yield 函数
    }   
}


void syscall_yield()
{
    _syscall0(SYS_CALL_YIELD);                  //  2 进入 _syscall0
}

static inline uint32_t _syscall0(uint32_t num)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"                           //  3  进入中断处理 _syscall_handler
        :"=a"(ret)
        :"a"(num));
    return ret;
}



_syscall_handler:

    push eax                

    call _syscall_check     

    pop eax                 

    push 0x2222_2222        
    
    push 0x80               

    push ds
    push es 
    push fs 
    push gs

    pusha                                   

    push 0x80               

    push edx                

    push ecx                

    push ebx                

    call [_syscall_table + eax * 4]         ;  4  这里调用  _syscall_table 中的对应的函数

    add esp, 12                             
    
    mov dword [esp + 8 * 4], eax            
                                        
    jmp interrupt_exit


void syscall_yield_handler()
{
    
    schedule();                             ;  5  终于进入 schedule
}



void schedule()
{
    
    task_t_new *current = running_task();
    task_t_new *next = task_search_in_table(TASK_READY);        

    assert(next != NULL);
    assert(next->magic == OS_MAGIC);

    if(current->state == TASK_RUNNING)          
    {
        current->state = TASK_READY;
    }

    if(current->ticks == 0)                     
    {
        current->ticks = current->priority;
    }
    next->state = TASK_RUNNING;

    task_switch(next);                      ;  6 进入 task_switch
}


_task_switch:                   
_task_switch_deprecated:
        push ebp
        mov ebp, esp           


        push ebx 
        push esi
        push edi

        mov eax, esp
        and eax, 0xffff_f000   
        mov [eax], esp                      ;  6.5.1 保存此时的栈

        mov eax, [ebp + 8]     
        mov esp, [eax]                      ;  6.5.2 更新栈
                                            


        pop edi
        pop esi
        pop ebx
        pop ebp
                               
        ret                                 ;  7 这里的 ret 会切到 B 的开始， 也是 6.5.2 更新栈 的位置
                               

uint32_t _ofp thread_b()
{
    set_if_flag(True);
    while(True)
    {
        printk("B");        
        syscall_yield();                    ;  B 的流程和之前一样 1 - 6.5.1 一样， 但是要注意，这时 6.5.2 更新的栈地址是 之前A 保存的地址
    }                                       ;  进而导致了，再次到 7 ret 的时候 回到的其实是 A 的 6 task_switch的结束， 进而是 schedule 结束，
                                            ;  syscall_yield_handler 结束， 进而 _syscall_handler 结束 interrupt_exit 结束 iret，int 0x80 结束
                                            ;  _syscall0 结束， syscall_yield 结束, 进而 开始打印下一轮的A， 如下所示，如此往复
}


uint32_t _ofp thread_a()    
{
    set_if_flag(True);
    while(True)
    {
        printk("A");
        syscall_yield();                       
    }   
}

比较乱的地方就是， 任务之间的  syscall_yield 切换 和 clock 的时间片的切换

但其实， clock 时钟 的 schedule 和 任务之间的 schedule 基本上是两种截然不同的 切换方式，只是融合在了一起，谁准备好了就 执行哪种切换

这里倒是有一些随机性
```

