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