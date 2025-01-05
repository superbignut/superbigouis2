[bits 32]

; global _interrupt_handler
    extern _handler_table

    %macro INTERRUPT_HANDLER 2

    _interrupt_handler_%1:

        %ifn %2
            push 0x2222_2222                ; 原本不压入错误码的，也压入一个
        %endif
            push %1                         ; 异常的编号
            jmp interrupt_entry
    %endmacro


interrupt_entry:

    mov eax, [esp]                          ; 异常的编号

    call [_handler_table + eax * 4]         ; 调用处理函数 回到 c 

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





;    extern _printk
;_interrupt_handler:
    
;    push msg            ;  传递给 printk的参数
    
;    call _printk

;    pop eax             ;  这里的 pop 比 add exp, 4 要好理解一点 要把push 的栈恢复
    
;    iret

;msg:
;    db "123123", 10, 0
