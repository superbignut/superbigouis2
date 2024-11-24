#### 看了github上的各个OS教程之后，会发现从零开始实现一个OS，真的是一个门槛非常高的领域：各种虚拟机软件，计算机名词，汇编等等。
#### 因此这个md文件就当作在学习各个教程时遇到的问题和解决办法的汇总，稳扎稳打，慢慢来。

1. [bochs](https://bochs.sourceforge.io/)

    bochs 是一个 x86 PC 模拟器，官网上说 bochs 可以模拟从 386 到 intel 和 amd 的 x86_64的多种 CPU ,因此和 VMware, Virtual Box, qemu 应该是同样的功能，但似乎 bochs调试上更优一些？

    对于bochs的安装，除了官网，OSDEV上也有详细的说明和[介绍](https://wiki.osdev.org/Bochs)。

    + bochs magic break
        
        > xchg bx, bx
    + 查看当前的10条反汇编指令
        > u/10 

    + 查看寄存器
        > sreg 或 r 

    + 查看ss:sp 堆栈
        > print-stack 

    + 产看10 * 4 bytes 的内存地址内容
        > xp/10


2. [bximage](https://bochs.sourceforge.io/doc/docbook/user/using-bximage.html)
    官网给出 bximage 是一个和 bochs 配套使用的磁盘镜像的创建、转换、解析工具。
    > Bximage is an easy to use console based tool for creating, converting and resizing disk images, particularly for use with Bochs. It also supports committing redolog files to their base images. It is completely interactive if no command line arguments are used. It can be switched to a non-interactive mode if all required parameters are given in the command line.

    在创建镜像文件的时候，会询问创建镜像的类型，flat, sparse, growing, vpc or vmware4:

    [官网指出](https://bochs.sourceforge.io/doc/docbook/user/harddisk-modes.html)：
    > In flat mode, all sectors of the harddisk are stored in one flat file, in lba order.

    在这里我们选用了flat类型，含义是可以将整个磁盘的不同存储区域当作一个逻辑上连续的空间进行访问。其余的类型则对应着不同的虚拟机。并且相对于growing，flat的特点是大小一次全部分配，而不是随着使用而增长。

3. [qemu](https://wiki.qemu.org/Main_Page)

    从功能上来看，qemu 和 bochs 类以，也是一个模拟器，下面[引用](https://people.redhat.com/pbonzini/qemu-test-doc/_build/html/topics/QEMU-compared-to-other-emulators.html)一段两者的区别，似乎在说，bochs 主打 x86 而 qemu 主打各种 CPU
    > Like bochs, QEMU emulates an x86 CPU. But QEMU is much faster than bochs as it uses dynamic compilation. Bochs is closely tied to x86 PC emulation while QEMU can emulate several processors.

4. [nasm assembler](https://www.nasm.us/)

    一个面向x86 CPU的汇编和反汇编工具，说白了就也是一个编译器，和gcc一样。
    
    + 问题4.1：使用下面的命令编译时，生成的指令的格式是什么？
  
        > nasm hello.asm -o nasm        

    首先 man手册提到，-f 参数的默认情况是 -f bin ; 
    
    其次根据[nasm手册7.1节](https://www.nasm.us/xdoc/2.16.02/html/nasmdoc0.html) 中提到的：

    >The most likely reason for using the BITS directive is to write 32-bit or 64-bit code in a flat binary file; this is because the bin output format defaults to 16-bit mode in anticipation of it being used most frequently to write DOS .COM programs, DOS .SYS device drivers and boot loader software.

    所以，bin的默认输出是16位指令，并且7.1节还对 BITS 的其他用法进行了说明，在os01的教程中也有提到。

5. [lba](https://en.wikipedia.org/wiki/Logical_block_addressing)
    
    Logical block addressing: 使用单一编号进行对不同磁盘分区进行索引，取代了传统的使用 cylinder-head-sector 方法。

6. [virtual-box](https://www.virtualbox.org/)

    同样也是一个虚拟机，相比与VMware的优点在于这个是开源的。由于我的电脑是ubuntu22.04,使用下面的命令可以成功安装：
    > sudo apt install virtualbox 
    
    > sudo apt install virtualbox-ext-pack

    最开始是从官网下载的deb包，但是发现后面在打开虚拟硬盘和安装拓展的时候总是报错。但回到 apt 进行安装就没什么问题。

7. [intel-8086](https://en.wikipedia.org/wiki/Intel_8086#cite_note-3)
    > The 8086 gave rise to the x86 architecture, which eventually became Intel's most successful line of processors.
    
    《x86汇编语言从实模式到保护模式》一书中大量引用了 8086 cpu 的各种特性，包括内存模型，初始加载地址，显存位置等等，似乎都是规定好的内容，但书中并没有给出出处，因此可能要自己探究一下。

    google 到的 [8086芯片手册](https://www.inf.pucrs.br/~calazans/undergrad/orgcomp_EC/mat_microproc/intel-8086_datasheet.pdf)给出了一些解释，但还有大篇幅的时序、引脚、模式的介绍暂时还不理解：

    > Direct Addressing Capability 1 MByte of Memory

    8086 有 1MB 也就是 2^20B 的存储空间

    > The processor provides a 20-bit address to memory which locates the byte being referenced. The memory is organized as a linear array of up to 1 millionbytes, addressed as 00000(H) to FFFFF(H). The memory is logically divided into code, data, extra data, and stack segments of up to 64K bytes each, with each segment falling on 16-byte boundaries.

    这里的 64K 和 16-byte boundary 指的就是一个 64K 的内存段需要开始在某个可以被16位整除的位置。

    > Locations from address FFFF0H through FFFFFH are reserved for operations including a jump to the initial program loading routine. Following RESET, the CPU will always begin execution at location FFFF0H where the jump must be. 

    这里解释了，复位后 8086会去执行 FFFF0H 处的指令，而这个指令是一个跳转指令。

    > After this interval the 8086 operates normally beginning with the instruction in absolute location FFFF0H. The details of this operation are specified in the Instruction Set description of the MCS-86 Family User’s Manual. 

    这里指出，有关启动的设计需要看[MCS-86 Family User’s Manual](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf), 这里也去看一下：

    > Chapter1 describes the architecture of the 8086 family, Chapter2 describes the 8086 and 8088 CPU, Chapter3 covers Input/Output Processor.

    具体的有关CPU的内容是在第二章

    > In addition, during periods when the EU(Exection unit) is busy executing instructions, the BIU(Bus Interface Unit) looks ahead and featches more instructions from memory.

    这部分在说8086没有使用传统的流水线的执行指令的方式，而是增加了一个BIU单元， 类似一个队列的数据结构

    > The CS register points to the current code segment; The SS register points to the current stack segment; The DS register points to the current data segment; The ES register points to the current extra segemnts; The 16-bit IP(instruction pointer) is updated by the BIU so that it constans the offset of the next instruction from the beginning of the current code segment;

    介绍了一些寄存器，当然后面还介绍了FLAG和寄存器和FLAG的关系。

    > It is useful to think of every memory location as having two kinds of addresses, pyhsical and logical.Pyhsical addresses may range from 0H through FFFFFH.
    Programs deal with logical, rather than pyhsical addresses and allow code to be developed without prior knowledge of where the code is to be located in memory and facilitate dynamic management of memory resources.

    介绍了逻辑地址和物理地址。

    > Two areas in extreme low and high memory are dedicated to specific processor functions or are reserved by Intel. The locations are OH-7FH(128 bytes) and FFFF0H-FFFFFH(16 bytes) Theres areas are used for interrupt and system reset processing.

    大致的提到了8086的两块保留内存。

    > THe 8086 can access either 8 or 16 bits of memory at a time. If an instruction refers to a word variable and variable is located at an even-numbered address, 8086 accesses the complete word in one bus cycle, odd-numbered address -> one byte.

    说到了有关存储大小的内容，后面还有对齐和I/O映射的内容暂时略过。

    这个手册里可以找到好多教科书上的内容和设计，但没有看到显卡的地址说明，可能还要继续仔细的读一下....

    查了google, 0xB8000 似乎就是一个在某种模式下的显示器会读取的 80 * 25 * 2 = 4000 bytes 首地址,有待进一步学习。
    + 这里在OSDEV的[Memory map](https://wiki.osdev.org/Memory_Map_(x86))中有提到;
    + 在[BDA - BIOS Data Area - PC Memory Map](https://stanislavs.org/helppc/bios_data_area.html),也有给出详细的从0x0000到0xFFFF:E的所有地址的用途。
    + 在踌躇月光的[x86视频02](https://www.bilibili.com/video/BV1b44y1k7mT?p=2&vd_source=5ad68ece2cc478b800d0c26152ca85c7)中也给出了更详细的地址布局，但不太清除出处。
    + 在踌躇月光的[x86视频03](https://www.bilibili.com/video/BV1b44y1k7mT?p=3&spm_id_from=pageDriver&vd_source=5ad68ece2cc478b800d0c26152ca85c7)中介绍到了0x7c00=31kB地址的历史，具体好像还要追溯到IBM-PC-5150, 待扩充

8. 先使用 bximage 制作了一块虚拟硬盘.img，然后使用 dd 命令将512字节的.bin 文件写入 .img , 为什么 bochs 就能够直接加载这个硬盘中的指令呢？同样的问题，为什么使用 qemu convert 转换为 vdi格式后，virtual_box也能直接打开呢？不需要一些对硬盘格式的说明吗？
    
        dd if=hello.bin of=master.img bs=512 count=1 conv=notrunc
        
        qemu-img convert -f raw -O vdi master.img master.vdi

    这里似乎最开始都默认是16位启动，似乎还和实模式和保护模式有关，以后再看....而且好像是除了qemu之外的这几个虚拟机都是只能模拟x86的，所以也就是默认按照x86的模式进行启动和加载，但是qemu可以模拟其他的架构。

9. [nasm 编译器语法](https://www.nasm.us/xdoc/2.16.02/html/nasmdoc0.html)

    在写最简单的第一个在屏幕上显示字母的汇编程序时，用到了很多奇怪的语法: times, db, $ ,$$之类，在查找NASM官网时看到[Chapter 3: The NASM Language](https://nasm.us/doc/nasmdoc3.html)对这些语法进行了解释，比如：

    > NASM supports two special tokens in expressions, allowing calculations to involve the current assembly position: the $ and $$ tokens. $ evaluates to the assembly position at the beginning of the line containing the expression; so you can code an infinite loop using JMP $. $$ evaluates to the beginning of the current section; so you can tell how far into the section you are by using ($ - $$).

    
        label:    instruction operands        ; comment

    > NASM places no restrictions on white space within a line: labels may have white space before them, or instructions may have no space before them, or anything. The colon after a label is also optional. 

    + ORG : [Binary File Program Origin](https://www.nasm.us/xdoc/2.16.02/html/nasmdoc8.html)
    
    > The function of the ORG directive is to specify the origin address which NASM will assume the program begins at when it is loaded into memory.

    + EQU： Defining Constants
    > EQU defines a symbol to a given constant value: when EQU is used, the source line must contain a label. The action of EQU is to define the given label name to the value of its (only) operand. This definition is absolute, and cannot change later.

    也就是说这是一个编译器的假设，假设程序的首地址，因此当在程序中涉及到具体的地址的时候，这个假设的偏移量都会被加上去。
10. div 指令

    同样的，书上在计算时，用到了 div指令，在intel [手册上](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)找到了相关内容,其中写到了当div 的操作数是不同格式时的指令描述：

        DX : AX / 16 = AX ... DX    AX / 8 = AL ... AH

    >  The action of this instruction depends on the operand size (dividend/divisor). Division using 64-bit operand is available only in 64-bit mode.

    并且学到一个表示方法：r/m8, r/m16,r/m32,这种写法指的是, 一个 8 位的通用寄存器（register）或者一个 8 位的内存地址（memory location） 

11. jmp, call, ret 指令

    jmp指令：

    > Transfers program control to a different point in the instruction stream without recording return information.

    intel 手册中指出了4中jump的形式：

    + Near jump : A jump to an instruction within the current code segment (the segment currently pointed to by the CS register) which means: -2^16 <=> 2^16-1
    + Short jump : A near jump where the jump range is limited to –128 to +127 from the current EIP value.
    + Far jump : A jump to an instruction located in a different segment than the current code segment but at the same privilege level
    + Task switch : A jump to an instruction located in a different task.
    
    这里如果不使用 near, short 进行显示标明的话, 猜测编译器会进行判断, 或者当作绝对地址使用

    感觉call指令和jmp指令的区别就是，是否保留当前的环境数据。

    call指令：
    > Saves procedure linking information on the stack and branches to the called procedure specified using the target operand. 

    ret指令：
    > Transfers program control to a return address located on the top of the stack. The address is usually placed on the stack by a CALL instruction, and the return is made to the instruction that follows the CALL instruction.

    loop指令:

    loop指令会默认让 cx寄存器减1，减到0循环终止。

    > Performs a loop operation using the RCX, ECX or CX register as a counter. Each time the LOOP instruction is executed, the count register is decremented, then checked for 0. If the count is 0, the loop is terminated and program execution continues with the instruction following the LOOP instruction. If the count is not zero, a near jump is performed to the destination (target) operand, which is presumably the instruction at the beginning of the loop.

    cmp指令：

    cmp指令虽然也是执行减法，但是不会保存结果到寄存器中。

    > Compares the first source operand with the second source operand and sets the status flags in the EFLAGS register according to the results. The comparison is performed by subtracting the second operand from the first operand and then setting the status flags in the same manner as the SUB instruction. 
    
12. 实模式和保护模式

    参考intel手册, OSDEV

    > Real Mode is a simplistic 16-bit mode that is present on all x86 processors. Real Mode was the first x86 mode design and was used by many early operating systems before the birth of Protected Mode. For compatibility purposes, all x86 processors begin execution in Real Mode.

    所以x86 启动时，为了考虑兼容性，最开始都会是实模式，也就是16位。

13. x86寄存器 [参考MCS-86 Family User’s Manual](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)

    + 通用寄存器：DATA_GROUP : AX BX CX DX 
    + 通用寄存器：POINTER_AND_INDEX_GROUP : SP BP SI DI
    
        MCS-86 Family User’s Manual还指出，有些指令会隐式的使用特定的寄存器,比如循环的loop会用到cx;

    + 段寄存器 : CS DS SS ES
    + Instruction Pointer : IP
    + One-Bit-FLAGS : TF DF IF OF SF ZF AF PF CF 
    
14. makefile 特殊字符
    
    make的一些特殊的字符在[GNU make 10.5](https://www.gnu.org/software/make/manual/make.html#Automatic-Variables)中有细致的说明：

    + $< 
    > The name of the first prerequisite. If the target got its recipe from an implicit rule, this will be the first prerequisite added by the implicit rule
    + $@
    > The file name of the target of the rule. If the target is an archive member, then ‘$@’ is the name of the archive file.

    + %
    > A target pattern is composed of a ‘%’ between a prefix and a suffix, either or both of which may be empty. The pattern matches a file name only if the file name starts with the prefix and ends with the suffix, without overlap.

15. x86寻值方式-寄存器简介寻址
    
    参考手册2.8节 [ADDRESSING MODE](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)
    给出了多种寻址方式，并发现涉及寄存器做偏移的只用到了bx,bp,si,di这四个，但是bp寄存器的默认段寄存器是ss和其他默认段寄存器的说明暂时没有看到。

    > A programmer may specify that either BX or BP is to serve as a base register whose content is to be used in the EA computation. Similarly, either SI or DI may be specified as an index register.

    这里进行补充一下,虽然很多听起来很复杂的寻值名词，但是参考手册2.8节的图2-34给出了寻值的总体样貌:
        
        cs/ds/ss/es + bx/bp/si/di || (bx/bp + si/di) + displacement

    虽然主要就是这三部分组成。但是实际在使用指令比如mov的时候，还是很乱，比如：

        mov ax, es:[bx]
        mov ax, [es:bx]
        mov ax, [bx]
    这三个似都是可以编译的，如下：

        0000104a: (                    ): mov ax, word ptr es:[bx]  ; 268b07
        0000104d: (                    ): mov ax, word ptr es:[bx]  ; 268b07
        00001050: (                    ): mov ax, word ptr ds:[bx]  ; 8b07

    可以发现前两个竟然是一样的，因此大概明白了

    此外还有就是当es/ds/ss/cs为目的操作数时，源操作数一定得是寄存器，记住就好

    然后就是实模式的2^20字节的寻值的限制，如果访问超过这个范围的地址，会被报错：

        mov eax, 0xf000
        mov es, eax
        mov ebx, 0xffff # 0x1ffff就会报错
        mov al, [es:ebx]
        


16. x86-算术运算指令

    + add
    
    > Adds the destination operand (first operand) and the source operand (second operand) and then stores the result in the destination operand. The destination operand can be a register or a memory location; the source operand can be an immediate, a register, or a memory location. (However, two memory operands cannot be used in one instruction.) When an immediate value is used as an operand, it is sign-extended to the length of the destination operand format.

    + adc

    > Adds the destination operand (first operand), the source operand (second operand), and the carry (CF) flag and stores the result in the destination operand. The destination operand can be a register or a memory location; the source operand can be an immediate, a register, or a memory location. (However, two memory operands cannot be used in one instruction.) The state of the CF flag represents a carry from a previous addition. When an immediate value is used as an operand, it is sign-extended to the length of the destination operand format.
    
    在add和adc中都提到了[sign-extended](https://en.wikipedia.org/wiki/Sign_extension), 指的是和Zero-extended 不同的另一种拓展方式，可以保证拓展后的符号不变，最高位是0就增加0,是1就增加1，但是这里都是以补码作为前提。

    + mul 
    
    > Performs an unsigned multiplication of the first operand (destination operand) and the second operand (source operand) and stores the result in the destination operand. The destination operand is an implied operand located in register AL, AX or EAX (depending on the size of the operand); the source operand is located in a general-purpose register or a memory location. T

        Unsigned multiply (AX := AL ∗ r/m8).

        Unsigned multiply (DX:AX := AX ∗ r/m16).

    + div

    > Divides unsigned the value in the AX, DX:AX, EDX:EAX, or RDX:RAX registers (dividend) by the source operand (divisor) and stores the result in the AX (AH:AL), DX:AX, EDX:EAX, or RDX:RAX registers. The source operand can be a general-purpose register or a memory location. The action of this instruction depends on the operand size (dividend/divisor). 

        DX : AX / 16 = AX ... DX    AX / 8 = AL ... AH

    + sbb

    > Adds the source operand (second operand) and the carry (CF) flag, and subtracts the result from the destination operand (first operand). The result of the subtraction is stored in the destination operand. The destination operand can be a register or a memory location; the source operand can be an immediate, a register, or a memory location.

        DEST := (DEST – (SRC + CF));

    + clc

    > The CF flag is set to 0. The OF, ZF, SF, AF, and PF flags are unaffected.

    + EFLAGS
        
    用#表示占位的话，可以看到16位的[flag寄存器](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)的功能如下：

    > ####OF DF IF TF SF ZF # AF # PF # CF  

17. x86 条件转移指令      

    start:
        jump short start 
    
    被编译成 EB FE , 总共占2个字节, 而FE是-2, 可以跳转-128 <=> 127

    start:
        jump near start 

    被编译成 E9 FD FF, 占3个字节, 0XFFFD是-3, 可以跳转 -2^16 <=> 2^16 -1

    start:
        jmp 0x0:0x7c00 ; far
    
    被编译成 EA 00 7C 00 00, 占5个字节

18. 堆栈和函数

    ss: sp 作为栈的地址，在函数调用的时候，sp-2 变为新的栈顶， 然后将函数调用之后的那个地址放入栈顶

        loop1:
            call print
            loop loop1

        print:

            mov al, [ds:si] ;

            mov [es:di], al ;

            add si, 1
            add di, 2
            ret
    比如这里就是在call print时， 将loop loop1的地址压入新的-2后的栈顶。而在print返回时，ret指令又会将栈顶元素拿出来交给cs: ip 


19. 内中断和异常，callf 和 retf
    
    在intel手册V1-Chapter6和INC指令处有介绍调用、异常和中断

    + int 指令
    
    > The INT n instruction generates a call to the interrupt or exception handler specified with the destination operand (see the section titled “Interrupts and Exceptions” in Chapter 6 of the Intel® 64 and IA-32 Architectures Software Developer’s Manual, Volume 1). The destination operand specifies a vector from 0 to 255, encoded as an 8-bit unsigned intermediate value. Each vector provides an index to a gate descriptor in the IDT. The first 32 vectors are reserved by Intel for system use. Some of these vectors are used for internally generated exceptions.


    > The vector specifies an interrupt descriptor in the interrupt descriptor table (IDT); that is, it provides index into the IDT. The selected interrupt descriptor in turn contains a pointer to an interrupt or exception handler procedure. In protected mode, the IDT contains an array of 8-byte descriptors, each of which is an interrupt gate, trap gate, or task gate. In real-address mode, the IDT is an array of 4-byte far pointers (2-byte code segment selector and a 2-byte instruction pointer), each of which point directly to a procedure in the selected segment. (Note that in real-address mode, the IDT is called the interrupt vector table, and its pointers are called interrupt vectors.)

    在使用call 和call far 时，分别会将ip和cs、ip入栈，而在 int 的时候则是将 cs、ip、EFLAG三个入栈;对应的也就需要ret、retf和iret分别将对应数量的寄存器出栈。

    如果要注册自己的中断函数的话，如下,对第80个中断向量注册了中断处理函数print,也就是要手动处理这4个字节的内容，当然处理函数需要使用iret进行返回操作

            mov word [0x80 * 4], print  ;ip
            mov word [0x80 * 4 + 2], 0  ;cs
            int 0x80                    ;调用自定义中断

    然后就是，有时需要显示使用 call far 指令才是callf

        call far [ds:print] ;callf
        call 0x0:print      ;callf

20. 逻辑运算指令

    AND TEST OR NOT XOR SHL SHR ROL ROR RCL RCR

    > Shifts (rotates) the bits of the first operand (destination operand) the number of bit positions specified in the second operand (count operand) and stores the result in the destination operand. The destination operand can be a register or a memory location; the count operand is an unsigned integer that can be an immediate or a value in the CL register. The count is masked to 5 bits (or 6 bits if in 64-bit mode and REX.W = 1).The rotate left (ROL) and rotate through carry left (RCL) instructions shift all the bits toward ore-significant bit positions, except for the most-significant bit, which is rotated to the least-significant bit location. 
    
    > The rotate right (ROR) and rotate through carry right (RCR) instructions shift all the bits toward less significant bit positions, except for the least-significant bit, which is rotated to the most-significant bit location. The RCL and RCR instructions include the CF flag in the rotation. 
    
    > The RCL instruction shifts the CF flag into the least-significant bit and shifts the most-significant bit into the CF flag. The RCR instruction shifts the CF flag into the most-significant bit and shifts the least-significant bit into the CF flag. 

    RCL、RCR 和 ROL、ROR的最大的区别就是是否要将进位参与到循环移位中，防止出错，最开始要将进位清零-CLC


21. 输入和输出

    这里要涉及到VGA的一些内容，[osdev](https://wiki.osdev.org/VGA_Hardware)有介绍，但内容真的很多，就不打算深入进去了

    >  The VGA has a lot (over 300!) internal registers, while occupying only a short range in the I/O address space. To cope, many registers are indexed. This means that you fill one field with the number of the register to write, and then read or write another field to get/set the actual register's value.

    这句话的意思就是说，需要使用索引的方式来访问寄存器，一个用来放地址，一个用来放数据；有很多可以用的索引寄存器，其中

    + CRT 地址端口 0x3d4

    + CRT 数据端口 0x3d5

    就是常用的索引寄存器，但为什么不是其他的，暂时不清楚，可能这个就是负责显示的吧，进而控制光标的位置

    + 0x0E-光标位置高8位

    + 0x0F-光标位置底8位

    也就是需要将这两个索引号，放到索引-地址-寄存器，然后去写或者读相应的索引-数据-寄存器，即可控制光标

    然后是要认清端口号的概念，intel手册卷1第19章：

    > The processor’s I/O address space is separate and distinct from the physical-memory address space. The I/O address space consists of 216 (64K) individually addressable 8-bit I/O ports, numbered 0 through FFFFH. I/O port addresses 0F8H through 0FFH are reserved. Do not assign I/O ports to these addresses. The result of an attempt to address beyond the I/O address space limit of FFFFH is implementation-specific; see the Developer’s Manuals for specific processors for more details.

    因此对于上面提到的端口号0x3d4，并不等同于内存地址0x3d4，很大的区别再于我们写入和读取使用的指令不是mov而是in/out：

    > The register I/O instructions IN (input from I/O port) and OUT (output to I/O port) move data between I/O ports and the EAX register (32-bit I/O), the AX register (16-bit I/O), or the AL (8-bit I/O) register. The address of the I/O port can be given with an immediate value or a value in the DX register.

    in 和 out 指令都是将数据从 port : dx 中读出或写入,但是允许使用的寄存器只限于AX , AL

    + out
    > Copies the value from the second operand (source operand) to the I/O port specified with the destination operand (first operand). The source operand can be register AL, AX, or EAX, depending on the size of the port being accessed (8, 16, or 32 bits, respectively); the destination operand can be a byte-immediate or the DX register. Using a byte immediate allows I/O port addresses 0 to 255 to be accessed; using the DX register as a source operand allows I/O ports from 0 to 65,535 to be accessed.
    + in
    > Copies the value from the I/O port specified with the second operand (source operand) to the destination operand (first operand). The source operand can be a byte-immediate or the DX register; the destination operand can be register AL, AX, or EAX, depending on the size of the port being accessed (8, 16, or 32 bits, respectively). Using the DX register as a source operand allows I/O port addresses from 0 to 65,535 to be accessed; using a byte immediate allows I/O port addresses 0 to 255 to be accessed.

        CRT_ADDR_REG equ 0x3d4
        CRT_DATA_REG equ 0x3d5
        CRT_CURSOR_HIGH equ 0x0e    
        CRT_CURSOR_LOW equ 0x0f

        set_cursor:                 ;设置光标函数
            ; ax传递参数
            push dx
            push bx
            mov bx, ax              ; bx = ax
            ;先设置地址寄存器，把光标索引地址传入
            mov dx, CRT_ADDR_REG    
            mov al, CRT_CURSOR_LOW
            out dx, al              ;写入port：dx
            ;再设置数据寄存器，把数据传入
            mov dx, CRT_DATA_REG    
            mov al, bl
            out dx, al              ;set bl
            mov dx, CRT_ADDR_REG
            mov al, CRT_CURSOR_HIGH
            out dx, al
            mov dx, CRT_DATA_REG
            mov al, bh
            out dx, al              ;set bh
            pop bx
            pop dx
            ret
        get_cursor:                 ;获取光标函数
            ; 结果存在ax
            push dx
            mov dx, CRT_ADDR_REG
            mov al, CRT_CURSOR_HIGH
            out dx, al
            mov dx, CRT_DATA_REG
            in al, dx               ; in data_port to al
            shl ax, 8
            mov dx, CRT_ADDR_REG
            mov al, CRT_CURSOR_LOW
            out dx, al
            mov dx, CRT_DATA_REG
            in al, dx
            pop dx
            ret
    但是我这里总是会有部分光标无法显示出来，不知道为什么

        print:
            call get_cursor ;获取初始位置的光标
            mov di, ax  ; ax: 0 1 2 3 4 5 光标显示的位置 * 2 + 0xb8000就是字符显示的位置
            shl di, 1   ; 指向下一个非样式的显示位置 di = ax * 2: 0 2 4 6 8 10
            mov bl, [ds:si]    ;获得字母h-e-l-l-o-,- -w-o-r-l-d
            cmp bl, 0       ;与空字符进行比较
            jz print_end
            mov [es:di], bl ;将字符放进 0xb8000 + di
            inc si
            inc ax
            call set_cursor ;设置新的光标
            jmp print
        print_end:

22. 外中断和时钟
    
    有很多概念第一次涉及到：

    + PIC : 8259 Programmable Interrupt Controller 
    > Every time the CPU is done with one machine instruction, it will check if the PIC's pin has notified an interrupt.    
    
    中断通过PIC 向CPU提交中断请求与中断向量，8259 PIC采用级联的方式，提供了15个中断，并通过数据线和命令线控制PIC

    > Each chip (master and slave) has a command port and a data port (given in the table below). When no command is issued, the data port allows us to access the interrupt mask of the 8259 PIC.

      + Master PIC - Command	0x0020  指令端口，用于ICW初始化和EOI等OCW命令

      + Master PIC - Data	0x0021  数据端口，如果要写入mask也是这个端口

      + Slave PIC - Command	0x00A0

      + Slave PIC - Data	0x00A1
    
    对PIC的控制则包括，初始化，设置屏蔽字等, 工作流程：

    > When the processor accepts the interrupt, the master checks which of the two PICs is responsible for answering, then either supplies the interrupt number to the processor, or asks the slave to do so. The PIC that answers looks up the "vector offset" variable stored internally and adds the input line to form the requested interrupt number. After that the processor will look up the interrupt address and act accordingly (see Interrupts for more details).

    OSDEV的介绍还是不太全面，需要结合[8259手册](https://pdos.csail.mit.edu/6.828/2017/readings/hardware/8259A.pdf)进行查看:

    手册上首页的框图给出了8259主要的结构：
      + Inservice Reg - 存储cpu正在服务的中断
      + Priority Resolver - 将IRR的最优先的中断传进ISR
      + Interrupt Request Reg - 并带有IR0~IR7 共8个输入引脚，连接各中断设备， 表示有那些中断进入
      + Inrerrupt Mask Reg - 屏蔽字
      + Control Logic - 输入引脚INTA用来让CPU获取中断向量，输出引脚INT用于向CPU触发中断
      + Data bus buffer - D0~D7 用于控制、状态、中断向量的数据的传输
      + Read/Write Logic - 包括用于初始化的ICW寄存器s，用于控制的OCW寄存器s
      + Cascade Buffer - 级联功能

    > The interrupts at the IR input lines are handled by two registers in cascade, the Interrupt Request Register (IRR) and the In-Service (ISR). The IRR is used to store all the interrupt levels which are requesting service; and the ISR is used to store all the interrupt levels which are being serviced.


    看了OSDEV下面的参考链接，大致包括：顺序的初始化ICW1~4,OCW1写屏蔽字，OCW2写EOI，但是很多地方很不理解

      + 首先是初始化并不是必须的，up在没有初始化的情况下，直接写屏蔽字也没有问题
      + 中断向量表和这个8259好像还是冲突的，如果在不初始化的情况下，IRQ0计数器中断用的就是0x08的中断向量，这块区域貌不应该动
      + ICW的初始化还是顺序执行的，也就是对一个端口进行多次的out
      + OCW2的写EOI(End of Interrupt)操作，out的第一个参数同样是0x20，0x20指令端口执行的是那个指令不仅和指令有关，还和指令触发的时间有关

    然后是sti和cli指令：

    > In most cases, STI sets the interrupt flag (IF) in the EFLAGS register. This allows the processor to respond to maskable hardware interrupts. If IF = 0, maskable hardware interrupts remain inhibited on the instruction boundary following an execution of STI. (The delayed effect of this instruction is provided to allow interrupts to be enabled just before returning from a procedure or subroutine. For instance, if an STI instruction is followed by an RET instruction, the RET instruction is allowed to execute before external interrupts are recognized. No interrupts can be recognized if an execution of CLI immediately follow such an execution of STI.) The inhibition ends after delivery of another event (e.g., exception) or the execution of the next instruction.

    也就是说sti指令会set可屏蔽中断的标志位if，因此允许接受可屏蔽的中断，并且在中断函数的执行过程中，会有cs、ip、flag进栈。并且这时的flag的if位又变成了unset的状态。

    所以总结一下就是：
      + sti可以打开eflag中cpu对可屏蔽中断的响应
      + 8259的mask会屏蔽来自设备的中断
      + 当8259中断一次后，所有中断都会被mask，发送EOI会解除这些mask
    
    
23. 硬盘读写

    对ATA的介绍OSDEV中并不是那么完善，比如当要写0x1f7时，命令的种类在[OSDEV：ATA](https://wiki.osdev.org/ATA_PIO_Mode)中就没有详细的介绍，
    并且对于28和48两种类型的LBA的区分也没有提及。
    
    更细节的介绍参考[ATA标准手册](http://ebook.pldworld.com/_eBook/ATA%20spec/ATA7_Spec.pdf)，比如第6章的指令介绍，

    + 6.16节 IDENTIFY DEVICE 0xEC
    + 6.35节 READ SECTORS 0x20
    + 6.67节 WRITE SECTORS 0x30
    
    卷二中更是提到的400ns的出处，还有很多状态转移框图，如果深入学习的话，都需要看一下

    下面是master硬盘驱动的端口号，范围是： 0x1f0 - 0x1f7 
    + 0x1f1 错误寄存器， 暂不使用
    + 0x1f2 要读写的扇区的数量
    + 0x1f3 ~ 0x1f5 LBA的情况下是扇区号的前24位：0-23 分别是 low-mid-high
    + 0x1f6 
      + 0 ~ 3 LBA扇区号的24-27位
      + 4 drive-number : 0->master ; 1 -> slave 用来选择硬盘
      + 6 :  0 -> chs; 1->lba 用来选择方式，如果是lba，指令则可以使用28和48两种
      + 5,7: 固定是 1
    + 0x1f7
      + 写入的情况，也就是命令寄存器：
        + 0xEC : 识别硬盘
        + 0x20 : 读硬盘
        + 0x30 : 写硬盘
      + 读取的情况，也就是状态寄存器：
        + 0 ERR
        + 1,2 ： 0
        + 3 DRQ  数据准备完毕 Set when the drive has PIO data to transfer, or is ready to accept PIO data. 
        + 4 SRV
        + 5 DF
        + 6 RDY
        + 7 BUSY 硬盘繁忙 Indicates the drive is preparing to send/receive data.

    代码写完后有一些疑问：
       1. nop指令
       > This instruction performs no operation. It is a one-byte or multi-byte NOP that takes up space in the instruction stream but does not impact machine context, except for the EIP register.

       但是具体会耗时多少的细节没有提及,而是google的说法都是不建议用nop作为延迟，做延迟的话也是1/f级别的延迟

       2. 什么时候需要nop     

       OSDEV上的说法是，每次发送一个指令后，都需要等待一段时间：
       >Which means that a drive select may always happen just before a status read. This is bad. Many drives require a little time to respond to a "select", and push their status onto the bus. 

       3. 硬盘是怎么判断我成功读了一个字节，并准备下一个字节的
       
       这里也没有找到，这个应该也是在ATA标准中，而且就比如在read的循环中:
                
            .check_read_state:  ; 选择扇区后要延迟一下
                nop
                nop
                nop
                in al, dx ; 0x1f7
                and al, 0b1000_1000
                cmp al, 0b0000_1000
                jnz .check_read_state

            mov ax, 0x100   ; 把数据读到0x1000
            mov es, ax
            mov di, 0
            mov dx, 0x1f0

            read_loop:          ;读取数据
                nop
                nop
                nop
                in ax, dx
                mov [es:di], ax
                add di, 2
                cmp di, 512
                jnz read_loop
            xchg bx, bx
        
    读取数据之前即使没有确认数据准备完毕，也可以进行写一个字的读取，这个时序猜测是硬件规范中规定的，并且这里如果硬盘响应的比cpu的频率慢，肯定的是会出错的

24. 内核加载器

    用到了pushad，popad指令
    > Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.

    然后就是再次套娃，用内存中的MBR再将一个新的更大容量的数据读入内存

    1. BIOS 加载512字节的 MBR 进入内存
    2. 内存中的 MBR 加载 更大体积的 loader 进入内存

25. 32位架构-实模式和保护模式

    > The memory management facilities of the IA-32 architecture are divided into two parts: segmentation and paging. Segmentation provides a mechanism of isolating individual code, data, and stack modules so that multiple programs (or tasks) can run on the same processor without interfering with one another. Paging provides a mechanism for implementing a conventional demand-paged, virtual-memory system where sections of a program’s execution environment are mapped into physical memory as needed. Paging can also be used to provide isolation between multiple tasks. When operating in protected mode, some form of segmentation must be used. There is no mode bit to disable segmentation. The use of paging, however, is optional.

    保护模式和实模式的一个很大的区别就是，他们的内存管理方式。实模式的时候，一直在使用段+偏移的方式来操作1Mb的地址;但是在保护模式下，还可以采用分页的方式进一步的管理地址的使用，使用的似乎就是MMU;

    如同上面的从[intel手册第三卷第三章](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)提到的，分段是为了隔离不同程序的代码和数据（因此8086的分段和这里的分段就不再是同一个目的了，8086是为了用16根地址线访问20位的地址 ; 而分页是为了提供虚拟内存，并进行了一个逻辑地址到物理地址的转换。

    这也就对应上了408中忘了是计组还是os的内容了。原来出处在这里，以后再来看。

    [nasm : section](https://www.nasm.us/xdoc/2.13.02rc3/html/nasmdoc7.html)

    section 的作用似乎就是进行对齐操作，并且对于不同的输出格式，也有不同的意义：

    + 7.1.2 bin Extensions to the SECTION Directive

    > The bin output format extends the SECTION (or SEGMENT) directive to allow you to specify the alignment requirements of segments. This is done by appending the ALIGN qualifier to the end of the section-definition line. For example,

    + 7.9.2 elf extensions to the SECTION Directive

    > Like the obj format, elf allows you to specify additional information on the SECTION directive line, to control the type and properties of sections you declare. Section types and properties are generated automatically by NASM for the standard section names, but may still be overridden by these qualifiers. 

    + section .text    progbits  alloc   exec    nowrite  align=16 
    
    + section .data    progbits  alloc   noexec  write    align=4 
    
    + section .bss     nobits    alloc   noexec  write    align=4 
    
    可以看到 elf 格式与 bin 格式相比，还多了属性上的约束,但是还不确定这些约束，比如可写、不可写是在编译阶段检查的吗？还是执行阶段？这里可以尝试写一下 section .text

    尝试了一下，如果写只读区域，会导致 core dump 的 error出现，所以猜测应该是将只读的section 放到特定的内存上，每当写只读区域的时候就会触发报错。


    [nasm : extern](https://www.nasm.us/xdoc/2.11.08/html/nasmdoc6.html)

    > EXTERN is similar to the MASM directive EXTRN and the C keyword extern: it is used to declare a symbol which is not defined anywhere in the module being assembled, but is assumed to be defined in some other module and needs to be referred to by this one. Not every object-file format can support external variables: the bin format cannot.

    [nasm : global](https://www.nasm.us/xdoc/2.11.08/html/nasmdoc6.html)

    > GLOBAL is the other end of EXTERN: if one module declares a symbol as EXTERN and refers to it, then in order to prevent linker errors, some other module must actually define the symbol and declare it as GLOBAL. Some assemblers use the name PUBLIC for this purpose.The GLOBAL directive applying to a symbol must appear before the definition of the symbol.

    extern 和 global 需要结合使用，任何使用extern的地方都需要找到，定义它为global的地方，进而进行链接操作

    [lea 指令]()
    
    > Computes the effective address of the second operand (the source operand) and stores it in the first operand(destination operand). The source operand is a memory address (offset part) specified with one of the processorsaddressing modes; the destination operand is a general-purpose register. The address-size and operand-size attributes affect the action performed by this instruction,

    + lea eax, [eax+2*eax]

    lea的第二个操作数是一个内存地址的偏移，也就是把[ ]中的值放入eax中 

    + mov eax, [eax+2*eax] 

    则是取出 eax+2*eax 内存地址中的值放入 eax

    所以说，结合来看，lea的括号和mov的括号的理解好像不太一样？

    在第[第3卷第21章]()也有介绍，手册里把实模式叫做“REAL-ADDRESS MODE”

    然后就是有一个误区，并不是在实模式下就不能访问32位的寄存器，比如eax，ebx这些，比如在bochs中模拟的32位x86，我们仍然可以使用e开头的这些寄存器。
    
    + 16位的限制似乎只体现在，我们在访存的时侯，用的cs/ds/ss/es是16位的，并且只能访问1Mb的空间。

    + 对于保护模式中，段寄存器似乎就被抛弃了，这时候32根线就是4GB的内存

26. 内存检测

    1. 这里其实有一个问题就是，bios 需要检测内存、硬盘等各种硬件设备，并将MBR的512个字节加载进入内存，所以我在MBR里面就不再需要重复这些操作了吗？
        + 但是好像是，bios检测内存后，os要能正确运行，也需要进行内存检测，但是这时就可以调用bios的函数了
    

    2. 第二个疑惑是，bios中断函数，也就是0x000-0x3FF这1KB的空间中的256的中断向量表的初始数据是谁写进去的呢? 就比如0x10, 0x15还有他们对应的中断处理函数，也是bios写的吗?
       + 猜测就是bios写的

    对 int 0x15, ax=0xe820 和 Address Range Descriptor Structure 的介绍只有这一个[网站](http://www.uruk.org/orig-grub/mem64mb.html)，是从osdev中跳转过去的, 但不太清楚权威性和正确性

    > Real mode only. This call returns a memory map of all the installed RAM, and of physical memory ranges reserved by the BIOS. The address map is returned by making successive calls to this API, each returning one "run" of physical address information. Each run has a type which dictates how this run of physical address range should be treated by the operating system. 

    这里说要不断的调用，然后可以不断的返回，不太理解。

    > If the information returned from INT 15h, AX=E820h in some way differs from INT 15h, AX=E801h or INT 15h AH=88h, then the information returned from E820h supersedes what is returned from these older interfaces. This allows the BIOS to return whatever information it wishes to for compatibility reasons.

    上文中指出，int 0x15 是bios内存检测函数不断演变的终极版，而其他的那些函数可能是为了兼容性就没有被废除。

    + Input: 
      + EAX	Function Code	E820h
      + EBX	Continuation	Contains the "continuation value" to get thenext run of physical memory.  This is the value returned by a previous call to this routine.  If this is the first call, EBX must contain zero.
  
	  + ES:DI	Buffer Pointer	Pointer to an  Address Range Descriptor	structure which the BIOS is to fill in.
	  + ECX	Buffer Size	The length in bytes of the structure passed	to the BIOS.  The BIOS will fill in at most ECX bytes of the structure or however much of the structure the BIOS implements.  The minimum size which must be supported by both the BIOS and the caller is 20 bytes.  Future implementations may extend this structure.
	  + EDX	Signature	'SMAP' -  Used by the BIOS to verify the caller is requesting the system map
				information to be returned in ES:DI.
	+ 输入:
	  + EAX 固定的

      + EBX 每次都使用上层调用之后的返回值，初始化是0
      + ES：DI 指向一个输出地址吗？
      + ECX 指定了最大的字节数，最小是20
      + EDX 验证的签名 SMAP字母的ascii
    + Output:
	  + CF Carry Flag   Non-Carry - indicates no error
	  + EAX	Signature	'SMAP' - Signature to verify correct BIOS revision.
	  + ES:DI Buffer Pointer	Returned Address Range Descriptor pointer. Same value as on input.
	  + ECX	Buffer Size	    Number of bytes returned by the BIOS in the address range descriptor.  The minimum size	structure returned by the BIOS is 20 bytes.
	  + EBX	Continuation	Contains the continuation value to get the next address descriptor.  The actual significance of the continuation value is up to the discretion of the BIOS.  The caller must pass the continuation value unchanged as input to the next iteration of the E820 call in order to get the next Address Range Descriptor.  A return value of zero means that this is the last descriptor.  Note that the BIOS indicate that the last valid descriptor has been returned by either returning a zero as the continuation value, or by returning carry.

    + 输出:
      + CF 标志是否发生错误
      + EAX 把字符串又返了回来
      + ES:DI 不变
      + ECX 返回填入的实际字节数
      + EBX 当最后一次调用时，EBX是0或者体现在CF上
    
    + Address Range Descriptor Structure 对返回的结构进行了描述
      + 0	    BaseAddrLow		Low 32 Bits of Base Address
	  + 4	    BaseAddrHigh	High 32 Bits of Base Address
	  + 8	    LengthLow		Low 32 Bits of Length in Bytes
	  + 12	    LengthHigh		High 32 Bits of Length in Bytes
	  + 16	    Type		    Address type of  this range. 
	   
    +  Type : 分别是被OS使用的和未使用的
         + 1       AddressRangeMemory      This run is available RAM usable by the operating system.
         + 2       AddressRangeReserved    This run of addresses is in use or reserved by the system, and must not be used by the operating system.
     
     
    + Assumptions and Limitations 列出了一些int0x15的局限性，暂不展开
    
      + 某些特殊的地址不会被返回
      + 某些地址应该由OS来进行检测也不返回
    
    +  这里在写'SMAP'的时候，如果直接写 
       +  mov eax, 'SMAP' 实际上move的是0x50414d53,也就是编译器把S放在了低位，因此int会触发CF报错
       +  mov eax, 0x534d4150 得到的与上面的相反，是正确的写法
   + jc 指令  
        > Jump short if carry (CF=1). 

   + 内存检测代码如下:

            check_memory:

                mov ax, 0
                mov es, ax              ;es = 0

                xor ebx, ebx            ;ebx = 0

                mov edx, 0x534d4150     ;edx = 'SMAP'

                mov di, ards_buffer     ;di指向一个地址
            .next:                      ;循环标记
                mov eax, 0xe820         ;eax e820
                mov ecx, 20             ;只能写20
                int 0x15                ;中断调用

                jc .error               ;判断carry是否报错,cf是eflag的最低位

                add di, cx              ;地址移动20个
                inc word [ards_num]     ;统计数+1
                cmp ebx, 0              ;判断是不是0,是0结束,不用改
                jnz .next               ;循环
            
                mov cx, [ards_num]      ;看循环了几次
                mov si, 0               ;指针
                
            .show:                                  ;读取
                mov eax, [si + ards_buffer]         ;只读了低32位,也就是4个字节
                mov ebx, [si + ards_buffer + 8]     ;length
                mov edx, [si + ards_buffer + 16]    ;type
                add si, 20                          ;每次移动20个字节,读取数据
                loop .show                          ;循环读取
            .error:
                jmp $

            ards_num:
                dw 0
            ards_buffer: 

        有个问题是,在读取ards数据的时候,只读取了底32位,这里其实理解一下可以知道,32位能表示的数字其实是很恐怖的,以地址来说,32位是4G的内存指针,内存检测应该也检测不到这么大的范围之外吧暂时,在以后说不定会用的到高32位

27. 保护模式
    
    最先看[第三卷2.1-overview](), 这样可以对整体逻辑有清除的认识：

    > When operating in protected mode, all memory accesses pass through either the global descriptor table (GDT) or an optional local descriptor table (LDT) as shown in Figure 2-1. These tables contain entries called segment descriptors. Segment descriptors provide the base address of segments well as access rights, type, and usage information.

    > Each segment descriptor has an associated segment selector. A segment selector provides the software that uses it with an index into the GDT or LDT (the offset of its associated segment descriptor), a global/local flag (deter-mines whether the selector points to the GDT or the LDT), and access rights information.

    2.1.1第一句话就直接给出了保护模式下的内存访问方式：

        memory-access -> GDT/LDT -> segment selector -> segment descriptors 

    > Protected mode is the main operating mode of modern Intel processors (and clones) since the 80286 (16 bit). On 80386s and later, the 32 bit Protected Mode allows working with several virtual address spaces, each of which has a maximum of 4GB of addressable memory; and enables the system to enforce strict memory and hardware I/O protection as well as restricting the available instruction set via Rings.

    这里提到了ring可能以后要留意一下

    > A CPU that is initialized by the BIOS starts in Real Mode. Enabling Protected Mode unleashes the real power of your CPU. However, it will prevent you from using most of the BIOS interrupts, since these work in Real Mode (unless you have also written a V86 monitor).

    提升性能，但是禁止了一些中断，有一些操作需要来打开保护模式，具体在[卷3-10.9节ModeSwitching]()有介绍：

    Before switching to protected mode, you must:

    1. Disable interrupts, including NMI (as suggested by Intel Developers Manual).
    2. Enable the A20 Line.
    3. Load the Global Descriptor Table with segment descriptors suitable for code, data, and stack.
    4. Execute a far JMP or far CALL instruction.
    5. The JMP or CALL instruction immediately after the MOV CR0 instruction changes the flow of execution and serializes the processor.
    
    Whether the CPU is in Real Mode or in Protected Mode is defined by the lowest bit of the CR0 or MSW register.

    进步进入保护模式是从硬件上判断的

    > Paging supports a “virtual memory” environment where a large linear address space is simulated with a small amount of physical memory (RAM and ROM) and some disk storage. 

    使用 RAM+ROM+DISK 来虚拟化线性地址空间

    > When a program (or task) attempts to access an address location in the linear address space, the processor uses the page directory and page tables to translate the linear address into a physical address and then performs the requested operation (read or write) on the memory location

    因此就需要一个页表来对虚拟地址到物理地址进行转换工作，进而如果所需要的page不在内存中，就会触发中断从硬盘把page读出来

    段访问有三种方式，分别对应了三种由低到高的硬件层面的保护：
       + basic-flat-model                
       + Protected Flat Model
       + Multi-Segment Model

    地址回绕- [memory wraparound](https://wiki.osdev.org/A20_Line)

    按道理来讲，如果我超出了 0xf_ffff 的话，环回后的地址应该就是 :
            
        address - 0xf_ffff -1   # 新的地址

    但是我总是环回不成功, 就是找不到0xee被写到哪里去了

        mov ax, 0xf000
        mov es, ax
        mov edi, 0x0000_ffff
        mov bx, 0x05ff

        mov byte [es : di + bx], 0xee # 可能还是哪里存在问题吧 

    +  A20 - 0x92 
    +  Protect Enable PE位 - cr0 

    + lgdt 指令 : lgdt指令就是告诉系统gdt表在那里
    > Loads the values in the source operand into the global descriptor table register (GDTR) or the interrupt descriptor table register (IDTR). The source operand specifies a 6-byte memory location that contains the base address (a linear address) and the limit (size of table in bytes) of the global descriptor table (GDT) or the interrupt descriptor table (IDT).  
   
    > There are several sources that enable A20, commonly each of the inputs are or'ed together to form the A20 enable signal. This means that using one method (if supported by the chipset) is enough to enable A20. If you want to disable A20, you might have to disable all present sources. Always make sure that the A20 has the requested state by testing the line as described above.

    [osdev](https://wiki.osdev.org/A20_Line)上说A20有很多种打开方式，任何一种都可以用，但是如果想关闭A20的话，就要disable所有来源。所以的话，是用0x92端口比较简单

            in al, 0x92     ; 打开A20
            or al, 0b10
            out 0x92, al
    但是这个方法的兼容性好像不是很好，还容易触发”危险“

    cr0第0位是PE位,置1开启保护模式

    >  Enables protected mode when set; enables real-address mode when clear. This flag does not enable paging directly. It only enables segment-level protection. To enable paging, both the PE and PG flags must be set.
    

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


    这里有个问题就是 jmp code_selector : protect_enable  ; 竟然可以直接跳到protect_enable的地方 <!-- 这里可以发现如果和于渊的书对比的话，代码在整体上没有用SECTION进行标记，因此按照nasm网给出的解释：    > Any code which comes before an explicit SECTION directive is directed by default into the .text section.    所有不标记的全是代码段，所以全部的代码段应该就是 -->

    这里其实就是对应手册[3.2.2 Protected Flat Model]()中指出的平坦保护模式，代码段和数据段的基地址都是0, 也就是他们的指向全部都是4G空间的0x0000的位置，并且没有界限，因此也就退化成了普通的平坦模式。而我们在加载loader的时候，loader是被加载到内存的0x1000的位置的，protect_enable 自然也会偏移到一个例如 0x10YY的位置上, 我的是0x1060。因此总的偏移就是
    0x0: 0x1060, 所以再使用0x0: 0x1060的时候会跳到正确的位置上。

    之前一直疑惑的地方是，总觉得 protect_enable 没有被显示的放到 gdt的index=1的那个代码段里，但是后来发现，因为指定了代码段的起始地址为0x0，极限为4GB, 因此0X1000很自然的就在这个超长的段里。终于明白了，有时候卡住了，慢慢想确实是好办法。

---
    好像是写太多了，vscode 打开有时候会崩溃，正好也进入保护模式了，换新的md文档了PROTECTED.md