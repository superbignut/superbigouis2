[实模式](https://wiki.osdev.org/Real_Mode)

参考intel手册, OSDEV

> Real Mode is a simplistic 16-bit mode that is present on all x86 processors. Real Mode was the first x86 mode design and was used by many early operating systems before the birth of Protected Mode. For compatibility purposes, all x86 processors begin execution in Real Mode.

> Cons:
    + Less than 1 MB of RAM is available for use.
    + There is no hardware-based memory protection (GDT), nor virtual memory.
    + There is no built in security mechanisms to protect against buggy or malicious applications.
    + The default CPU operand length is only 16 bits.
    + The memory addressing modes provided are more restrictive than other CPU modes.
    + Accessing more than 64k requires the use of segment register that are difficult to work with.

这里指出x86 启动时，为了考虑兼容性，最开始都会是实模式，也就是16位。

> Common Misconception: 

> Programmers often think that since Real Mode defaults to 16 bits, that the 32 bit registers are not accessible. This is not true.All of the 32-bit registers (EAX, ...) are still usable, by simply adding the "Operand Size Override Prefix" (0x66) to the beginning of any instruction.Your assembler is likely to do this for you, if you simply try to use a 32-bit register.

这里说明了，虽然是实模式，但是仍然可以使用32位寄存器，只是在编译的时候，会在指令前面加上0x66

[x86寄存器:参考MCS-86 Family User’s Manual](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)

+ 通用寄存器：DATA_GROUP : AX BX CX DX
+ 通用寄存器：POINTER_AND_INDEX_GROUP : SP BP SI DI

    MCS-86 Family User’s Manual还指出，有些指令会隐式的使用特定的寄存器,比如循环的loop会用到cx;

+ 段寄存器 : CS DS SS ES
+ Instruction Pointer : IP
+ One-Bit-FLAGS : TF DF IF OF SF ZF AF PF CF

[实模式的寻址模式](osdev)

> Addressing Modes
> Real Mode uses 16-bit addressing mode by default. Assembly programmers are typically familiar with the more common 32-bit addressing modes, and may want to make adjustments -- because the registers that are available in 16-bit addressing mode for use as "pointers" are much more limited. The typical programs that run in Real Mode are often limited in the number of bytes available, and it takes one extra byte in each opcode to use 32-bit addressing instead.
> Note that you can still use 32-bit addressing modes in Real Mode, simply by adding the "Address Size Override Prefix" (0x67) to the beginning of any instruction. Your assembler is likely to do this for you, if you simply try to use a 32-bit addressing mode. But you are still constrained by the current "limit" for the segment that you use in each memory access (always 64K in Real Mode -- Unreal Mode can be bigger).

        [BX + val]
        [SI + val]
        [DI + val]
        [BP + val]
        [BX + SI + val]
        [BX + DI + val]
        [BP + SI + val]
        [BP + DI + val]
        [address]

这里的意思似乎是说，寻址的时候也可以用32位寄存器，但是无法查过2**16 也就是64kB的一个段的限制，osdev 给出的寻址 和 下面的manual中给出的是一致的



[x86寻值方式-寄存器简介寻址](manual)

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