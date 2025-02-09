[bits 32]

; global _interrupt_handler
    extern _handler_table

    %macro INTERRUPT_HANDLER 2

    _interrupt_handler_%1:

        %ifn %2
            push 0x2222_2222                ; 原本不压入错误码的，也压入一个
        %endif
            push %1                         ; 异常的编号，也当作了传给c的函数的参数
            jmp interrupt_entry
    %endmacro


interrupt_entry:

    
    push ds
    push es 
    push fs 
    push gs

    pusha                                   ;  EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    mov eax, [esp + 4 * 12]                 ; 不不不，这里的 eax 只是为了计算下面的第几个回调函数 拿到异常编号

    push eax                                ; 这个参数 就是 异常（中断）编号

    call [_handler_table + eax * 4]         ; 调用处理函数 回到 c 

    global interrupt_exit

interrupt_exit:

    pop eax                                 ; 这里能走回来吗？？？

    popa

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8                              ; 弹出两个参数

    iret



                                            ; 宏定义，负责使用宏 来定义 _interrupt_handler_0x** 函数
    INTERRUPT_HANDLER 0x00, 0
    INTERRUPT_HANDLER 0x01, 0
    INTERRUPT_HANDLER 0x02, 0
    INTERRUPT_HANDLER 0x03, 0
    INTERRUPT_HANDLER 0x04, 0
    INTERRUPT_HANDLER 0x05, 0
    INTERRUPT_HANDLER 0x06, 0
    INTERRUPT_HANDLER 0x07, 0
    INTERRUPT_HANDLER 0x08, 1
    INTERRUPT_HANDLER 0x09, 0
    INTERRUPT_HANDLER 0x0A, 1
    INTERRUPT_HANDLER 0x0B, 1
    INTERRUPT_HANDLER 0x0C, 1
    INTERRUPT_HANDLER 0x0D, 1               ; 一般性保护异常
    INTERRUPT_HANDLER 0x0E, 1
    INTERRUPT_HANDLER 0x0F, 0
    INTERRUPT_HANDLER 0x10, 0
    INTERRUPT_HANDLER 0x11, 1               ; 
    INTERRUPT_HANDLER 0x12, 0
    INTERRUPT_HANDLER 0x13, 0
    INTERRUPT_HANDLER 0x14, 0
    INTERRUPT_HANDLER 0x15, 1
    INTERRUPT_HANDLER 0x16, 0
    INTERRUPT_HANDLER 0x17, 0
    INTERRUPT_HANDLER 0x18, 0
    INTERRUPT_HANDLER 0x19, 0
    INTERRUPT_HANDLER 0x1A, 0
    INTERRUPT_HANDLER 0x1B, 0
    INTERRUPT_HANDLER 0x1C, 0
    INTERRUPT_HANDLER 0x1D, 1
    INTERRUPT_HANDLER 0x1E, 1
    INTERRUPT_HANDLER 0x1F, 0

    INTERRUPT_HANDLER 0x20, 0               ; 时钟中断，可能是给到了晶振？
    INTERRUPT_HANDLER 0x21, 0               ; 键盘中断
    INTERRUPT_HANDLER 0x22, 0
    INTERRUPT_HANDLER 0x23, 0
    INTERRUPT_HANDLER 0x24, 0
    INTERRUPT_HANDLER 0x25, 0
    INTERRUPT_HANDLER 0x26, 0
    INTERRUPT_HANDLER 0x27, 0
    INTERRUPT_HANDLER 0x28, 0
    INTERRUPT_HANDLER 0x29, 0
    INTERRUPT_HANDLER 0x2A, 0
    INTERRUPT_HANDLER 0x2B, 0
    INTERRUPT_HANDLER 0x2C, 0
    INTERRUPT_HANDLER 0x2D, 0
    INTERRUPT_HANDLER 0x2E, 0
    INTERRUPT_HANDLER 0x2F, 0
                   

    global _handler_entry_table             ; 将 _interrupt_handler_0x** 函数的首地址 放在一个数组中， 并声明为 global

_handler_entry_table:                       ;  这里就相当于把 各个处理函数的首地址放在一块， 当作一个数组
    dd _interrupt_handler_0x00              ;  4个字节
    dd _interrupt_handler_0x01
    dd _interrupt_handler_0x02
    dd _interrupt_handler_0x03
    dd _interrupt_handler_0x04
    dd _interrupt_handler_0x05
    dd _interrupt_handler_0x06
    dd _interrupt_handler_0x07
    dd _interrupt_handler_0x08
    dd _interrupt_handler_0x09
    dd _interrupt_handler_0x0A
    dd _interrupt_handler_0x0B
    dd _interrupt_handler_0x0C
    dd _interrupt_handler_0x0D
    dd _interrupt_handler_0x0E
    dd _interrupt_handler_0x0F
    dd _interrupt_handler_0x10      
    dd _interrupt_handler_0x11
    dd _interrupt_handler_0x12
    dd _interrupt_handler_0x13
    dd _interrupt_handler_0x14
    dd _interrupt_handler_0x15
    dd _interrupt_handler_0x16
    dd _interrupt_handler_0x17
    dd _interrupt_handler_0x18
    dd _interrupt_handler_0x19
    dd _interrupt_handler_0x1A
    dd _interrupt_handler_0x1B
    dd _interrupt_handler_0x1C
    dd _interrupt_handler_0x1D
    dd _interrupt_handler_0x1E
    dd _interrupt_handler_0x1F

    dd _interrupt_handler_0x20      
    dd _interrupt_handler_0x21
    dd _interrupt_handler_0x22
    dd _interrupt_handler_0x23
    dd _interrupt_handler_0x24
    dd _interrupt_handler_0x25
    dd _interrupt_handler_0x26
    dd _interrupt_handler_0x27
    dd _interrupt_handler_0x28
    dd _interrupt_handler_0x29
    dd _interrupt_handler_0x2A
    dd _interrupt_handler_0x2B
    dd _interrupt_handler_0x2C
    dd _interrupt_handler_0x2D
    dd _interrupt_handler_0x2E
    dd _interrupt_handler_0x2F


section .text

    extern _syscall_check, _syscall_table
    global _syscall_handler

    ;  系统调用处理函数，
_syscall_handler:

    push eax                ;  暂存 eax, 进行参数检查

    call _syscall_check     ;  也就是检查 系统调用号

    pop eax                 ;  返回值在 eax 寄存器中， 后续根据eax 的不同调用号调用不同的 处理函数

    push 0x2222_2222        ;  类比错误码
    
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

    call [_syscall_table + eax * 4]         ;  调用

    add esp, 12                             ;  三个参数的 栈的恢复， 这里之所以 不是 add esp, 16 是为了留一个 参数4， 进而兼容 interrupt_exit
    
    mov dword [esp + 8 * 4], eax            ;  把 eax 放回到 pusha 中的 eax, 进而在 popa 的时候，恢复到 eax 
                                            ;  从而作为 系统调用 的返回值

    jmp interrupt_exit


;    extern _printk
;_interrupt_handler:
    
;    push msg            ;  传递给 printk的参数
    
;    call _printk

;    pop eax             ;  这里的 pop 比 add exp, 4 要好理解一点 要把push 的栈恢复
    
;    iret

;msg:
;    db "123123", 10, 0
