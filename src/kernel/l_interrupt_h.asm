[bits 32]

global _interrupt_handler

extern _printk
_interrupt_handler:
    
    push msg            ;  传递给 printk的参数
    
    call _printk

    pop eax             ;  这里的 pop 比 add exp, 4 要好理解一点 要把push 的栈恢复
    
    iret

msg:
    db "123123", 10, 0
