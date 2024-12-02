[bits 32]


    extern _kernel_init ; windows 的函数需要加 "_"

    global _start
_start:

                        ; mov byte [0xb8000], 'K'
    ; xchg bx, bx
    call _kernel_init   ; 保护模式下的call, 不需要用 selector 的形式吗

    jmp $