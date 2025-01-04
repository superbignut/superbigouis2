    [bits 32]


    extern _kernel_init ; windows 的函数需要加 "_"

    global _start
_start:

    ; xchg bx, bx
    ; call test
                        ; mov byte [0xb8000], 'K'
    ; xchg bx, bx
    call _kernel_init   ; 保护模式下的call, 不需要用 selector 的形式吗

    ; xchg bx, bx
    int 0x80            ; 调用中断处理函数，然后返回

    jmp $
test_push_and_ret:   

    push $              ; 这种方式也可以实现 jmp $ 的效果
    ret