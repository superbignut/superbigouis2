[保护模式]()
    
> Protected mode is the main operating mode of modern Intel processors (and clones) since the 80286 (16 bit). On 80386s and later, the 32 bit Protected Mode allows working with several virtual address spaces, each of which has a maximum of 4GB of addressable memory; and enables the system to enforce strict memory and hardware I/O protection as well as restricting the available instruction set via Rings.

Osdev第一句，保护模式，赋予了系统严格的内存和硬件io的保护，并且，限制了可以使用的指令，释放了cpu的真正的能力。


最先看[第三卷2.1-overview](), 这样可以对整体逻辑有清除的认识：

> When operating in protected mode, all memory accesses pass through either the global descriptor table (GDT) or an optional local descriptor table (LDT) as shown in Figure 2-1. These tables contain entries called segment descriptors. Segment descriptors provide the base address of segments well as access rights, type, and usage information.


2.1.1第一句话就直接给出了保护模式下的内存访问方式：

    memory-access -> segment selector  -> GDT/LDT -> segment descriptors 

> Each segment descriptor has an associated segment selector. A segment selector provides the software that uses
    it with an index into the GDT or LDT (the offset of its associated segment descriptor), a global/local flag (deter-
    mines whether the selector points to the GDT or the LDT), and access rights information.
    
> To access a byte in a segment, a segment selector and an offset must be supplied. The segment selector provides
    access to the segment descriptor for the segment (in the GDT or LDT). From the segment descriptor, the processor
    obtains the base address of the segment in the linear address space. The offset then provides the location of the
    byte relative to the base address. This mechanism can be used to access any valid code, data, or stack segment,
    provided the segment is accessible from the current privilege level (CPL) at which the processor is operating. The
    CPL is defined as the protection level of the currently executing code segment.
> See Figure 2-1. The solid arrows in the figure indicate a linear address, dashed lines indicate a segment selector,
    and the dotted arrows indicate a physical address. For simplicity, many of the segment selectors are shown as
    direct pointers to a segment. However, the actual path from a segment selector to its associated segment is always
    through a GDT or LDT.
> The linear address of the base of the GDT is contained in the GDT register (GDTR); the linear address of the LDT is
    contained in the LDT register (LDTR).

这段文字基本就是统领了内存访问的整个过程。以前竟然没看到这么纲领的说明。


### 保护模式下，内存怎么分段，descriptor 的结构是什么样子，见[V3-Capter3]()，模式切换见[V3-Capter11]()

> A CPU that is initialized by the BIOS starts in Real Mode. Enabling Protected Mode unleashes the real power of your CPU. However, it will prevent you from using most of the BIOS interrupts, since these work in Real Mode (unless you have also written a V86 monitor).

提升性能，但是禁止了一些中断，有一些操作需要来打开保护模式，具体在[卷3-11.9节ModeSwitching]()有介绍：

[V3-11.9] 之前需要先看一下 [V3-11.8]() ，其中给出了切换到保护模式之前需要做的一些准备

具体的怎么切换，需要准备哪些工作还是要看这两节

[V3-11.8]()准备工作最少：
    + GDT
    + GDTR 寄存器
    + 代码段 + 一段保护模式下可以运行的代码    
    + cr1-cr4 寄存器

[V3-11.9]()切换工作：
    + 关中断 cli
    + （A20线打开）Intel手册中没写，但是需要加上
    + lgdt 加载gdt的基地址
    + cr0修改

+ 所以接下来就是围绕着准备工作和切换工作进行展开：
    + 准备工作：
      1. gdt 部分需要做的最多， 包括：
        + code_selector
        + code_descriptor * 3 : NULL + code + data  
        + 3 个 code_descriptor 构成了一个GDT表
         
      2. gdtr 是用来被lgdt 指令调用的参数 需要手动构造
       
      3.  一段代码段
          + 这部分其实还挺有说法的
          + 需要用[bits 32] 指定后面的代码是32位的代码, 具体在nasm.md说明
          + 保护模式启动后，需要跳转到保护模式的代码开始执行， 这时候已经要使用 selector ： offset 的模式了
          + 还有一个很重要的地方就是， 保护模式中 段寄存器的作用就变成了 存储 code_selector

+ 其实具体的操作也是这些，但是实现起来细节问题还是太多了，也就对着代码来能好一些

---
[segment_selector]()
```cpp
typedef struct segment_selector{
    RPL_2,
    TI_1,
    Index_13
} segment_selector;
```

segment_selector是一个16位的选择器
    + 高13位是用来作为序号选择gdt的segment_descriptor
    + TI位 0表示GDT， 1表示LDT
    + RPL-Requested Privilege Level位用来表示优先级与segment_descriptor的DPL结合使用


---
[segment_descriptor]()

```cpp
typedef struct segment_descriptor
{
    Segment_Limit_16[0 : 15],   // 界限
    Base_Address_24[0 : 23],    // 基地址
    Type_4,
    S_1,
    DPL_2,
    P_1,
    Segment_Limit_4[16 : 19],
    AVL_1,
    L_1,
    DB_1,
    G_1,
    Base_Address_8[24 : 31]
} segment_descriptor;
```


+ 低16位表示限制大小limit[0:15], 这里要注意的是

+ 紧接着的23位是 基地值 base[0:23], 就是这个segment_descriptor的首地址
+ 4位的type 表示的范围是0-15, 其中[0,7]表示数据段， [8,15]表示代码段， 具体的区别在于访问A，读写R/W和扩展方向E， 除了访问位作为一种标记手段，其余两个bit的作用会根据代码段和数据段而有所区别
+ S 位 1表示 代码段和数据段， 为0表示系统段，暂时不理解
+ DPL-descriptor privilege level 与上面的RPL结合使用
+ P 位表示 段是否被加加载到内存中
+ D_B 位对于32位需要置1
+ G 位 用来表示limit 的粒度，0代表字节位单位，1代表4kB为单位
+ 最高8为位也是基地址base[24:31]

---

[A20总线](https://wzh.one/253.html)

> 1981年8月，IBM公司最初推出的个人计算机IBM PC使用的CPU是Intel8088。在该微机中地址线只有20根(A0 – A19)。在当时，内存RAM只有几百KB或不到1MB时，20根地址线已足够用来寻址这些内存。

> 在8086/8088中，有20根地址总线，所以可以访问的地址是2^20=1M，但由于8086/8088是16位地址模式，能够表示的地址范围是0-64K，所以为了在8086/8088下能够访问1M内存，Intel采取了分段的模式：16位段基地址:16位偏移。其绝对地址计算方法为：16位基地址左移4位+16位偏移=20位地址。

> 通过上述分段模式，能够表示的最大内存为：FFFFh:FFFFh=FFFF0h+FFFFh=10FFEFh=1M+64K-16Bytes（1M多余出来的部分被称做高端内存区HMA）。可是在8086/8088中，当程序给出超过1M（100000H-10FFEFH）的地址时，系统并不认为其访问越界而产生异常，而是自动从重新0开始计算，环绕到0FFEFh，也就是说系统计算实际地址的时候是按照对1M求模的方式进行的，这种技术被称为wrap-around。

> 但是当时已经有一些程序是利用这种地址环绕机制进行工作的。为了实现完全的兼容性，IBM公司发明了使用一个开关(逻辑门)来开启或禁止100000h地址比特位。由于在当时的8042键盘控制器上恰好有空闲的端口引脚（输出端口P2，引脚P21），于是便使用了该引脚来作为与门控制这个地址比特位。该信号即被称为A20门。如果A20为零，则比特20及以上地址都被清除。从而实现了兼容性。毕竟A20Gate和键盘操作没有任何关系，在许多新型PC上存在着一种通过芯片来直接控制A20 Gate的BIOS功能。从性能上，这种方法比通过键盘控制器来控制A20Gate要稍微高一点。

> 当A20被禁止时：程序员给出100000H~10FFEFH间的地址，80286和8086/8088 的系统表现是一致的，即按照对1M求模的方式进行寻址，满足系统升级的兼容性问题；

> 当A20被开启时：程序员给出的100000H~10FFEFH间的地址，80286是访问的真实地址，而8086/8088是始终是按照对1M求模的方式进行的（这里注意，是始终）。

> 所以，在保护模式下，如果A20 Gate被打开，则可以访问的内存则是连续的；如果A20Gate被禁止，则可以访问的内存只能是偶数段，因为是20位（从0始）总为零，所23~20位只能是0000、0010、0100、0110、1000、1010、1100、1110对应十六进制为0、2、4、6、8、A、C、E。对应的十六进制地址段是000000-0FFFFF，200000-2FFFFF，400000-4FFFFF…。

> A20地址线并不是打开保护模式的关键，只是在保护模式下，不打开A20地址线，你将无法访问到所有的内存。

看明白了，这个标志位不修改的话，第20位总是0， 会导致内存不连续，中间少了一段， 所以要把这一位置1， 才可以访问超出20位的内存


[Fast A20 Gate](https://wiki.osdev.org/A20_Line)
> On most newer computers starting with the IBM PS/2, the chipset has a FAST A20 option that can quickly enable the A20 line. To enable A20 this way, there is no need for delay loops or polling, just 3 simple instructions.

            in al, 0x92
            or al, 2
            out 0x92, al

up 使用这个方式打开的A20， 所以bochsrc看起来是支持的

>  Enables protected mode when set; enables real-address mode when clear. This flag does not enable paging directly. It only enables segment-level protection. To enable paging, both the PE and PG flags must be set.

cr0第0位是PE位,置1开启保护模式

> Immediately following the MOV CR0 instruction, execute a far JMP or far CALL instruction. (This operation is typically a far jump or call to the next instruction in the instruction stream.)

CR0 修改后， jump 到保护模式

> As with segments, the limit value is added to the base address to get the address of the last valid byte. A limit value of 0 results in exactly one valid byte. Because segment descriptors are always 8 bytes long, the GDT limit should always be one less than an integral multiple of eight (that is, 8N – 1).

这里是在说，因为gdt的第一个descriptor不能用， 因此 limit 要再真实的limit 的基础上 - 1 


用#表示占位的话，可以看到16位的[flag寄存器](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)的功能如下：

> ####OF DF IF TF SF ZF # AF # PF # CF  EFLAG 标志位
    + OF 溢出
    + DF 方向
    + IF 中断
    + TF 陷阱标志
    + SF 负数
    + ZF 零
    + AF 辅助进位
    + PF 奇偶
    + CF 进位



<!-- 

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




 -->