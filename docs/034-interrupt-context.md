```cpp

uint32_t _ofp thread_a()    
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

```

#### 有几个比较模糊的点：

1. 这里的中断的上下文保存的寄存器更多 但是 cs 和ss 没有被保存
2. _ofp 忽略栈帧的作用
3. IF标志位会被中断自动修改吗
4. 中断 schedule 的总体的流程是什么


---

1. 这里断点去找了一下，发现 cs 位在loader第一次进入保护模式的时候，就被自动修改了：

```asm
    ; xchg bx, bx   ; 在这里 cs 被默认修改成了 code_selector 的数值
    jmp code_selector : protect_enable      ; 跳到保护模式
```


2.  __attribute__((optimize("omit-frame-pointer"))) 忽略栈帧

    确实是这样的，可以用一个简单的函数看出来，如果加了这个，push ebp 的字段就没有了

    ```cpp
    
    int __attribute__((optimize("omit-frame-pointer"))) func()
    {
        return 1;
    }
    int main()
    {
        func();
    }

    ```


    ```asm
    _func:
        movl	$1, %eax
        ret
        .def	___main;	.scl	2;	.type	32;	.endef
        .globl	_main
        .def	_main;	.scl	2;	.type	32;	.endef
    _main:
        pushl	%ebp
        movl	%esp, %ebp
        call	___main
        call	_func
        movl	$0, %eax
        popl	%ebp
        ret


    ```


3. 是的，手册有提到 [7.12.1.3 Flag Usage By Exception- or Interrupt-Handler Procedure]()

    > The only difference between an interrupt gate and a trap gate is the way the processor handles the IF flag in the
    EFLAGS register. When accessing an exception- or interrupt-handling procedure through an interrupt gate, the
    processor clears the IF flag to prevent other interrupts from interfering with the current interrupt handler. A subse-
    quent IRET instruction restores the IF flag to its value in the saved contents of the EFLAGS register on the stack.
    Accessing a handler procedure through a trap gate does not affect the IF flag.

    这里的意思就是中断门在发生中断时， 会清空 IF 位， 并且 iret 会恢复 if 位



4. 中断 shcedule 的总体流程：

```cpp
uint32_t _ofp thread_a()        
{
    asm volatile("sti");       1. 这里打开中断
    while(True)
    {
        printk("A");
        delay(40000);          2. 在循环中会被中断切入，进而跳入 中断处理函数(同时，被cpu 清除 IF 位)，
    }
}

```

```asm

interrupt_entry:

    
    push ds
    push es 
    push fs 
    push gs

    pusha                                   

    mov eax, [esp + 4 * 12]                 

    push eax                                

    call [_handler_table + eax * 4]         3. 中断汇编压入错误码和几乎全部上下文寄存器后，跳入中断c函数

    pop eax                                 下面的还没执行到，但call函数 会把 eip 压入栈，等到call的返回时被执行

    popa

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8                              

iret



```



```cpp
static void hardware_int_handler(int vector)
{
    send_eoi(vector);                                   4. 告诉 8259 本次中断结束
    schedule();                                         5. 开始调度打印B的代码
}


void schedule()
{
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    task_switch(next);                                  6. 进入 A 的 task_switch 准备切换
}

```


```asm

_task_switch:                   

        push ebp
        mov ebp, esp           


        push ebx 
        push esi
        push edi

        mov eax, esp
        and eax, 0xffff_f000                           6.5 thread_a 保存上下文
        mov [eax], esp                                  

        mov eax, [ebp + 8]     
        mov esp, [eax]                                 7. 这里拿到了 thread_b 的入口地址
                               


        pop edi
        pop esi
        pop ebx
        pop ebp

        ret                                            8. 开始执行 thread_b

```


```cpp

uint32_t _ofp thread_b()
{
    asm volatile("sti");                               9. 打开被 thread_a 中进入中断而关闭的 if 位
    while(True)
    {
        printk("B");
        delay(40000);                                  10. 循环中，会再次进入中断(清空 IF)
    }
}

```


``` asm
                                                        
_task_switch:                                           11.  再次重复 3 到 6， 并再次进入  _task_switch

        push ebp
        mov ebp, esp           


        push ebx 
        push esi
        push edi

        mov eax, esp
        and eax, 0xffff_f000   
        mov [eax], esp         

        mov eax, [ebp + 8]     
        mov esp, [eax]                                 12. 但是这里恢复的是 6.5 中 thread_a 的 _task_switch 保存的上下文
                               


        pop edi
        pop esi
        pop ebx
        pop ebp

        ret                                            13. 回到 a 的 _task_switch 需要返回的地方的eip 继续执行
                                                       14. 也就是 a 的 schedule
                                                       
                                                       15. 同理 a 的shedule 函数执行结束，继续返回 

                                                       16. 返回到 a 的 hardware_int_handler

                                                       17. 继续返回到 a 的 中断 汇编 interrupt_entry

                                                       interrupt_entry 的 最后一条指令 iret 正式 将 A 的中断代码执行结束 

                                                       开始回到 第一次中断的地方，继续打印 "A"
 

```