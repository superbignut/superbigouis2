    [org 0x1000]

    db 0x55, 0xaa ; 开始模式

    mov si, loading_os_str
    call real_printf

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 进行内存检测 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 内存检测结束 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    jmp prepare_for_protected_mode  ; 跳到保护模式准备阶段

;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 下面的代码用于显示哪些内存布局可用，进入保护模式则跳过 ;;;;;;;;;;;;;;;;;;
    mov word cx, [memory_detect_ards_num]               ; 有多少个 ards
    mov si, 0
    mov ax, 0
    mov es, ax

    ; xchg bx, bx

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


prepare_for_protected_mode:

    mov si, prepare_for_protected_mode_str
    call real_printf

    cli             ; 关闭中断

    in al, 0x92     ; 打开A20开关
    or al, 0b10
    out 0x92, al

    lgdt [gdtr_register]

    mov eax, cr0    ; 打开cr0-PE 进入保护模式
    or al, 1        ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax

    ; xchg bx, bx    
    jmp code_selector : protect_enable      ; 跳到保护模式

halt:
    jmp halt


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 实模式下打印 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 保护模式 开始 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[bits 32]
protect_enable:
    mov ax, data_selector       ; 切换到数据段
	mov ds, ax                  ; 这里的ds在保护模式下还是会用到， 真实没想到 
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
    mov esp, 0x10000

    ; mov byte ds:[0x110000], 0xab
    mov byte ds:[0xb8000], 'P'
    ; xchg bx, bx
    jmp $   


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; segment selector ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

code_selector equ (0x0001 << 3)               ; selector 右三位置0

data_selector equ (0x0002 << 3)               ; 其余为 index
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; gdt 开始 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdtr_register:                                              ; 需要加载到 gdtr 的内容 需要包括 16位limit + 32位地址 ，描述了gdt表的基本信息
    dw (gdt_end_address - gdt_base_address -1)              ; limit - 1 doc中有说明
    dd gdt_base_address                                     ; 基地址

segment_descriptor_base equ 0               ; 0 

segment_descriptor_limit equ 0xfffff        ; 2**20 * 4kB = 4GB

gdt_base_address:
    dd 0, 0                     ; 第一个空的segment-descriptor 8个字节

; 这里使用V3-3.2.1中提到的Basic Flat Model 即代码段和数据段都是4GB的空间来进行简单的划分
gdt_code_segment_descriptor:
    dw segment_descriptor_limit & 0xffff            ; limit[0:15]
    dw segment_descriptor_base & 0xffff             ; base[0:15]
    db (segment_descriptor_base >> 16) & 0xff                          ; base[16:23]
    db 0b1001_0000 | 0b1110                         ; D_7=4KB 
                                                    ; DPL_5_6=most privileged level 
                                                    ; S_4= code_or_data segment
                                                    ; Type_0_3 = Execute/Read, conforming
    db 0b1100_0000 | ( (segment_descriptor_limit >> 16) & 0xf )        ; G_7/DB_6/L_5/AVL_4/limit[16:19]_3_0
    db (segment_descriptor_base >> 24) & 0xff                          ; base[24:31]

gdt_data_segment_descriptor:
    dw segment_descriptor_limit & 0xffff
    dw segment_descriptor_base & 0xffff
    db (segment_descriptor_base >> 16) & 0xff                          
    db 0b1001_0000 | 0b0010                         ; D_7=4KB 
                                                    ; DPL_5_6=most privileged level 
                                                    ; S_4= code_or_data segment
                                                    ; Type_0_3 = Read/Write
    db 0b1100_0000 | ( (segment_descriptor_limit >> 16) & 0xf )
    db (segment_descriptor_base >> 24) & 0xff    


gdt_end_address:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 进入loader打印字符串 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
loading_os_str:
    db 'Loading os...', 10, 13, 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 保护模式辅助字段 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
prepare_for_protected_mode_str:
    db "Preparing P Mode...", 10, 13, 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 内存检测辅助字段 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
memory_detect_ards_num:     ; 用来统计内存检测返回了多少个ards
    dw 0
memory_detect_str:
    db "Detecting Memory...", 10, 13, 0
memory_detect_ards_buffer:  ; 存储内存检测 ards 的首地址

