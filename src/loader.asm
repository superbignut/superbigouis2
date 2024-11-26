    [org 0x1000]

    db 0x55, 0xaa ; 开始模式

    mov si, loading_str
    call real_printf

halt:
    jmp halt

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

boot_error:
    mov si, .msg
    call real_printf
    hlt
    .msg:
        db "Loading Error", 10, 13, 0
loading_str:
    db 'Loading os...', 10, 13, 0
    