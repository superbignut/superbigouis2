    [org 0x7c00] ;

    mov ax, 3   ; clear screen
    int 0x10    ; clear screen

    mov ax, 0
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov sp, 0x7c00

    mov edi, 0x1000 ; 将硬盘的第2个扇区(lba=2)开始的4个扇区，移到0x1000位置
    mov ecx, 2
    mov bl, 4
    

    call read_disk
    jmp 0: 0x1000
halt:                   
    jmp halt

read_disk:
    pushad          ;eax, ecx, ebx, edx, esp, ebp, esi, edi 这里如果是16位的栈的话，32位寄存器会压两次
    push es        
    ;读取硬盘
    ; es:edi - 把读取到的数据，在内存中存放的位置 edi
    ; ecx - 读取的原始数据在硬盘中的扇区位置-lba
    ; bl 扇区数量
    mov dx, 0x1f2 
    mov al, bl      ; 读写扇区的数量
    out dx, al

    mov al, cl      ; 读取第0个扇区 low
    mov dx, 0x1f3
    out dx, al

    shr ecx, 8
    mov al, cl
    mov dx, 0x1f4   ; mid
    out dx, al

    shr ecx, 8
    mov al, cl
    mov dx, 0x1f5   ; high
    out dx, al

    shr ecx, 8
    and cl, 0b0000_1111
    mov al, 0b1110_0000
    or al, cl
    mov dx, 0x1f6       ;master + lba_high_4
    out dx, al

    mov al, 0x20    ; read sectors command 0x20
    mov dx, 0x1f7
    out dx, al

    xor ecx, ecx
    mov cl, bl

    .read:
        push cx     ; cx was changed in .read_sector
        call .wait_sector   ; wait every read.
        call .read_sector
        pop cx
        loop .read

    pop es
    popad
    ret

    .wait_sector:
        mov dx, 0x1f7
        .check_read_state:  ; 选择扇区后要延迟一下，并不是每次读取时都要检查
            nop
            nop
            nop
            in al, dx ; 0x1f7
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check_read_state
        ret


    .read_sector:
        mov dx, 0x1f0
        mov cx, 256     ;loop one sector
        mov ax, 0
        mov es, ax      ; es set 0

        .read_loop:          ;读取数据
            nop
            nop
            nop
            in ax, dx   ; port = dx read port:dx to ax.
            mov [es:edi], ax    ; read to es:edi
            ; 如果不进入保护模式的话，这个edi只能从0x0000 加到 0xffff 也就是 65535 / 512 = 128
            ; 最多为 128个扇区
            add edi, 2           ; edi
            

            loop .read_loop
        ret



times 510 - ($ - $$) db 0   

db 0x55, 0xaa 
