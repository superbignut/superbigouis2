    [org 0x7c00]

    mov ax, 3
    int 0x10
    xchg bx, bx
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00


    ; 注册中断向量
    mov word [0x80 * 4], print  ;ip
    mov word [0x80 * 4 + 2], 0  ;cs
    
    ; 调用中断
    int 0x80    

    jmp $

    

print:
    mov ax, 0
    mov ds, ax
    mov ax, 0xb800
    mov ds, ax
    mov byte [ds:0], 'H'
    iret


    times 510 - ($-$$) db 0

    db 0x55, 0xaa