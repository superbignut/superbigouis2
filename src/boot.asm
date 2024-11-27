    [org 0x7c00]

    mov ax, 3
    int 0x10
    ; xchg bx, bx
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov si, booting_os_str
    call real_printf    

    mov edi, 0x1000     ; 加载地址 0x1000
    mov ecx, 0x2        ; 第2个扇区开始读
    mov bl, 0x4         ; 总共读4个扇区

    call read_disk
    

    cmp word [ds:0x1000], 0xaa55   ; 虽然存的是 db 0x55, 0xaa, 但是当作一个字节取出来， 就是0xaa55
    jnz boot_error

    ; xchg bx, bx
    jmp 0:0x1002        ; 如果写成0:0这种形式就是jmpf， 如果只是jmp 0x1002 就是普通的jmp
halt:
    jmp halt

boot_error:
    mov si, .msg
    call real_printf
    hlt
    .msg:
        db "Booting Error...", 10, 13, 0

read_disk:
    pushad          ;eax, ecx, ebx, edx, esp, ebp, esi, edi 这里如果是16位的栈的话，32位寄存器会压两次
    push es         ; 中间会把es 置0, 所以先存了一下
    
    ;               从硬盘中读取数据到内存中
    ; 0:edi         读取数据，在内存中存放的位置 edi
    ; ecx           读取的原始数据在硬盘中的扇区位置-lba, 也就是第几个扇区
    ; bl            扇区数量

    mov dx, 0x1f2 
    mov al, bl      ; 读写扇区的数量
    out dx, al      ; out 指令将al 写入对应端口寄存器

    mov al, cl      ; ecx 低位放到对应端口
    mov dx, 0x1f3   ; low
    out dx, al

    shr ecx, 8      ; ecx 右移8位, 中8位放到对应端口
    mov al, cl      
    mov dx, 0x1f4   ; mid
    out dx, al

    shr ecx, 8      ; ecx 右移8位, 高8位放到对应端口
    mov al, cl
    mov dx, 0x1f5   ; high
    out dx, al

    shr ecx, 8
    and cl, 0b0000_1111         ; 取出0~3位是 lba的最高位
    mov al, 0b1110_0000
    or al, cl                   ;; 567 位全是1 , 表示 lba28
    mov dx, 0x1f6               ;master + lba_high_4
    out dx, al

    mov al, 0x20                ; read sectors command 0x20
    mov dx, 0x1f7
    out dx, al                  ; 放到w:指令寄存器io

    xor ecx, ecx                ; 清空
    mov cl, bl                  ; cl = bl = 读取扇区的数量

    .read:
        push cx                 ; cx was changed in .read_sector
        call .wait_sector       ; wait every sector read. 这里相当于以扇区为单位进行读取
        call .read_sector
        pop cx                  ; 因为cx 要作为loop的index位, 所以要先存起来
        loop .read

    pop es
    popad
    ret

    .wait_sector:
        mov dx, 0x1f7
        .check_read_state:          
            nop
            nop
            nop
            in al, dx               ; 把 0x1f7 r:状态寄存器中的值 读出来
            and al, 0b1000_1000
            cmp al, 0b0000_1000     ; DRQ 表示数据准备完成 BUSY表示指令执行结束
            jnz .check_read_state   
        ret


    .read_sector:           ; 每次读取一个扇区 512个字节
        mov dx, 0x1f0       ; 把16位数据拿出来
        mov cx, 256         ; loop one sector
        mov ax, 0
        mov es, ax          ; es set 0

        .read_loop:         ;读取数据
            nop
            nop
            nop
            in ax, dx               ; port = dx read port:dx to ax.
            mov [es:edi], ax        ; read to es:edi, 一次是两个字节
                                    ; 如果不进入保护模式的话，这个edi只能从0x0000 加到 0xffff 也就是 65535 / 512 = 128
                                    ; 最多为 128个扇区
            add edi, 2              ; edi, edi是32为所以+2大概率不会超量程, 如果使用di就不一定了
            loop .read_loop         ; loop 会减少cx
        ret

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
        
booting_os_str:
    db 'Booting os...', 10, 13, 0

    times 510 - ($-$$) db 0
    db 0x55, 0xaa