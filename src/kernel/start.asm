    [bits 32]


    extern _kernel_init         ; windows 的函数需要加 "_"
    extern _ards_init         ; 内存初始化
    extern _console_init
    extern _gdt_init


    global _start
_start:
    
    push ecx            ; ards buffer 的首地址, 第三个参数
    push ebx            ; 可用的 ards 的数量， 第二个参数
    push eax            ; 保存魔数，第一个参数，三个参数都在 jmp 之前被保存

    call _console_init

    call _gdt_init

    call _ards_init
    
    call _kernel_init   

    jmp $
test_push_and_ret:   

    push $              ; 这种方式也可以实现 jmp $ 的效果
    ret