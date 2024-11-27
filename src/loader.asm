    [org 0x1000]

    db 0x55, 0xaa ; 开始模式

    mov si, loading_os_str
    call real_printf

;###################################### 进行内存检测 ##############################################
    mov si, memory_detect_str
    call real_printf
check_memory:                               

    mov ax, 0
    mov es, ax                                  ; es = 0

    xor ebx, ebx                                ; ebx = 0 ebx初始是0

    mov edx, 0x534d4150                         ; edx = 'SMAP' 

    mov di, memory_detect_ards_buffer           ; di指向一个地址
    .next:                                      ; 循环标记
        mov eax, 0xe820                         ; eax e820
        mov ecx, 20                             ; 20个字节
        int 0x15                                ; 中断调用

        jc .error                               ; 判断carry是否报错, cf是eflag的最低位

        add di, 20                              ; 地址移动20个
        inc word [memory_detect_ards_num]       ; 统计数+1
        cmp ebx, 0                              ; 判断是不是0,是0结束,不用改
    jnz .next                                   ; 循环



;###################################### 内存检测结束 ##############################################

    jmp $








;############################ 下面的代码用于显示哪些内存布局可用，进入保护模式则跳过 ##################
    mov word cx, [memory_detect_ards_num]               ; 有多少个 ards
    mov si, 0
    mov ax, 0
    mov es, ax

    xchg bx, bx

    .show:                                              ; 把检测的结果不断的加载到寄存器中，使用xchg查看
        mov eax, [memory_detect_ards_buffer + si]       ; 32bits 这里默认高32位全是0
        mov ebx, [memory_detect_ards_buffer + si + 8]   ; 32bits        
        mov edx, [memory_detect_ards_buffer + si + 16]  ; 32bits        
        add si, 20                                      ; si+=20
        
    loop .show
    ; 我的检测结果如下：type=1为可用区域
    ; 0x0_0000      len:0x9f000       type=1
    ; 0x9_f000      len:0x1000        type=2
    ; 0xe_8000      len:0x18000       type=2
    ; 0x10_0000     len:0x01ef_0000   type=1
    ; 0x1ff_0000    len:0x1_0000      type=3
    ; 0xfffc_0000   len:0x4_0000      type=3 
    
    ; 0x10_0000     l:0x1ef_0000    type=1 不到32MB的内存空间最大
    ; 所以bochs虚拟机里总共有两块内存可以使用
    .error:
        mov ax, 0xb800
        mov ax, es
        mov byte [es:0], 'E'  
        jmp $



halt:
    jmp halt


;###################################### 实模式下打印 ##############################################
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
;###################################### 进入loader打印字符串 #########################################
loading_os_str:
    db 'Loading os...', 10, 13, 0


;###################################### 内存检测辅助字段 ##############################################
memory_detect_ards_num:     ; 用来统计内存检测返回了多少个ards
    dw 0
memory_detect_str:
    db "Detecting Memory...", 10, 13, 0
memory_detect_ards_buffer:  ; 存储内存检测 ards 的首地址

