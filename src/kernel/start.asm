[bits 32]


    extern _kernel_init ; windows 的函数需要加 "_"

    global _start
_start:

                        ; mov byte [0xb8000], 'K'
    call _kernel_init   ; 保护模式下的call

    jmp $