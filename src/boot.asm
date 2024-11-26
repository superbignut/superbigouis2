    [org 0x7c00]

    mov ax, 3
    int 0x10
    xchg bx, bx
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov si, hello_str
    call real_printf

    jmp $

real_printf:
    ; si用于存放字符串首地址， 字符串用0表示结束
    mov cx, 0
    mov ds, cx
    mov ah, 0x0e
    
.next:
    mov byte al, [ds:si]
    cmp al, 0  
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

print:
    mov ax, 0
    mov ds, ax
    mov ax, 0xb800
    mov ds, ax
    mov byte [ds:0], 'H'
    iret

hello_str:
    db 'H', 'E', 'L', 'L', 'O', 0

    times 510 - ($-$$) db 0
    db 0x55, 0xaa