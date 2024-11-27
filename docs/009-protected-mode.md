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


> A CPU that is initialized by the BIOS starts in Real Mode. Enabling Protected Mode unleashes the real power of your CPU. However, it will prevent you from using most of the BIOS interrupts, since these work in Real Mode (unless you have also written a V86 monitor).

提升性能，但是禁止了一些中断，有一些操作需要来打开保护模式，具体在[卷3-11.9节ModeSwitching]()有介绍：

[V3-11.9] 之前需要先看一下 [V3-11.8]() ，其中给出了切换到保护模式之前需要做的一些准备

具体的怎么切换，需要准备哪些工作还是要看这两节

[V3-11.8]()准备工作最少：
    + GDT
    + 代码段 + 一段保护模式下可以运行的代码
    + GDTR 寄存器
    + cr1-cr4 寄存器

[V3-11.9]()切换工作：
    + 关中断 cli
    + （A20线打开）Intel手册中没写，但是需要加上
    + lgdt 加载gdt的基地址
    + cr0修改

> Immediately following the MOV CR0 instruction, execute a far JMP or far CALL instruction. (This operation is typically a far jump or call to the next instruction in the instruction stream.)

> The JMP or CALL instruction immediately after the MOV CR0 instruction changes the flow of execution and serializes the processor.
    + Execute the STI instruction to enable maskable hardware interrupts and perform the necessary hardware operation to enable NMI interrupts.


保护模式下，内存怎么分段，descriptor 的结构是什么样子，还要看[V3-Capter3]()

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

其中的TYPE 部分涉及到一个新的概念：[conforming]()

> Code segments can be either conforming or nonconforming. A transfer of execution into a more-privilegedconforming segment allows execution to continue at the current privilege level. A transfer into a nonconforming segment at a different privilege level results in a general-protection exception

值得就是，jmp到一个更高的优先级的代码，但是conforming 的则不报错， 并允许在原始优先级下执行，跳到nonconforming的代码段就会报错


+ [lgdt 指令](): lgdt指令就是告诉系统gdt表在那里
> Loads the values in the source operand into the global descriptor table register (GDTR) or the interrupt descriptor table register (IDTR). The source operand specifies a 6-byte memory location that contains the base address (a linear address) and the limit (size of table in bytes) of the global descriptor table (GDT) or the interrupt descriptor table (IDT).  

  
> As with segments, the limit value is added to the base address to get the address of the last valid byte. A limit value of 0 results in exactly one valid byte. Because segment descriptors are always 8 bytes long, the GDT limit should always be one less than an integral multiple of eight (that is, 8N – 1).

这里是在说，因为gdt的第一个descriptor不能用， 因此 limit 要再真实的limit 的基础上 - 1 

>  Enables protected mode when set; enables real-address mode when clear. This flag does not enable paging directly. It only enables segment-level protection. To enable paging, both the PE and PG flags must be set.

cr0第0位是PE位,置1开启保护模式


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

[cli]()

> In most cases, CLI clears the IF flag in the EFLAGS register and no other flags are affected. Clearing the IF flag
causes the processor to ignore maskable external interrupts. The IF flag and the CLI and STI instruction have no
effect on the generation of exceptions and NMI interrupts.


使用 cli 关闭可屏蔽中断，但是对不可屏蔽中断和异常没有影响


    jmp prepare_protect_mode


    prepare_protect_mode:

        cli             ; 关闭中断

        in al, 0x92     ; 打开A20
        or al, 0b10
        out 0x92, al    ; 写回

        lgdt [gdt_ptr]  ; 指定 gdt表 的起始地址和limit

        mov eax, cr0
        or eax, 1
        mov cr0, eax    ; 进入保护模式

        jmp code_selector : protect_enable  ; 竟然可以直接这么跳的吗？
        
        ; code_selector 和 protect_enable 是怎么产生联系的？

        ud2                 ; 触发异常， 正常情况下跳过执行

    [bits 32]               ; 不太清楚为啥这么写
    protect_enable:

        mov ax, data_selector           
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax
        mov esp, 0x10000

        mov byte [0xb8000], 'P'     ;显示字母    

        mov byte [0x200000], 'P'    ;写入内存

        xchg bx, bx

        jmp $

    base equ 0
    limit equ 0xfffff           ;20bit

    code_selector equ (0x0001 << 3)  ; index = 1 选择gdt中的第一个
    data_selector equ (0x0002 << 3)  ; index = 2 选择gdt中的第二个


    ;gdt 描述地址
    gdt_ptr:                       ; 6B at all
        dw (gdt_end - gdt_base -1) ; 2B limit limit = len - 1
        dd gdt_base                ; 4B base GDT基地址

    gdt_base:
        dd 0, 0 ; 8B 第一个Segment Descriptor是空的
    gdt_code:
        dw limit & 0xffff           ;limit[0:15]
        dw base & 0xffff            ;base[0:15]
        db (base >> 16) & 0xff      ;base[16:23]
        ;type
        db 0b1110 | 0b1001_0000     ;D_7/DPL_5_6/S_4/Type_0_3 代码段
        db 0b1100_0000 | ( (limit >> 16) & 0xf )   ;G_7/DB_6/L_5/AVL_4/limit[16:19]_3_0
        db (base >> 24) & 0xff      ;base[24:31]

    gdt_data:
        dw limit & 0xffff
        dw base & 0xffff
        db (base >> 16) & 0xff
        ;type
        db 0b0010 | 0b1001_0000
        db 0b1100_0000 | (limit >> 16)
        db (base >> 24) & 0xff    

    gdt_end:


这里有个问题就是 jmp code_selector : protect_enable  ; 竟然可以直接跳到protect_enable的地方

[jmp]() 在Intel手册中的jmp文档部分的后面其实有介绍 使用jmp 跳到保护模式，但是看的模模糊糊的，不清楚



这里其实就是对应手册[3.2.2 Protected Flat Model]()中指出的平坦保护模式，代码段和数据段的基地址都是0, 也就是他们的指向全部都是4G空间的0x0000的位置，并且没有界限，因此也就退化成了普通的平坦模式。而我们在加载loader的时候，loader是被加载到内存的0x1000的位置的，protect_enable 自然也会偏移到一个例如 0x10YY的位置上, 我的是0x1060。因此总的偏移就是
0x0: 0x1060, 所以再使用0x0: 0x1060的时候会跳到正确的位置上。

之前一直疑惑的地方是，总觉得 protect_enable 没有被显示的放到 gdt的index=1的那个代码段里，但是后来发现，因为指定了代码段的起始地址为0x0，极限为4GB, 因此0X1000很自然的就在这个超长的段里。终于明白了，有时候卡住了，慢慢想确实是好办法。

---
好像是写太多了，vscode 打开有时候会崩溃，正好也进入保护模式了，换新的md文档了PROTECTED.md