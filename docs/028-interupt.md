异常：
    异常执行完处理函数会回到原异常位置继续执行

中断：
    这里参考intel手册卷三第七章 INTERRUPT AND EXCEPTION HANDLING 感觉说的还挺多的，但肯定还要结合up 的代码才是老少皆宜

> This chapter describes the interrupt and exception-handling mechanism when operating in protected mode on an Intel 64 or IA-32 processor. 

可以看到最开始就指出了，第七章的全部内容都是保护模式下的， 重点！！！

> In x86 protected mode the table is called the Interrupt Descriptor Table (IDT) and can have up to 256 entries, but the name of this table and the maximum number of entries it can have can differ based on the CPU architecture. After the CPU finds the entry for the interrupt, it jumps to the code the entry points to. This code that is run in response to the interrupt is known as a interrupt service routine (ISR) or an interrupt handler.

中断向量 -> 中断描述符

中断向量表 -> 中断描述符表

参考 osdev 看到一个流程，不知道具体实施起来是不是这样的，先列下来：

> So how do I program this stuff?

> Step by step, now that you've grabbed the whole thing and know what's to be done:

+ Make space for the interrupt descriptor table
+ Tell the CPU where that space is (see GDT Tutorial: lidt works the very same way as lgdt)
+ Tell the PIC that you no longer want to use the BIOS defaults (see Programming the PIC chips)
+ Write a couple of ISR handlers (see Interrupt Service Routines) for both IRQs and exceptions
+ Put the addresses of the ISR handlers in the appropriate descriptors (in Interrupt Descriptor Table)
+ Enable all supported interrupts in the IRQ mask (of the PIC)


此外，还在7.11节 跳到了，6.8.3 的 call gates， 感觉说的是 怎么手动调用一些任务门 task gate 这里也是up 提到的64位中删掉的内容

但是看到一些介绍：

> Call gates facilitate controlled transfers of program control between different privilege levels. They are typically
used only in operating systems or executives that use the privilege-level protection mechanism. Call gates are also
useful for transferring program control between 16-bit and 32-bit code segments, as described in Section 23.4,
“Transferring Control Among Mixed-Size Code Segments.”

> It defines the size of values to be pushed onto the target stack: 16-bit gates force 16-bit pushes and 32-bit
gates force 32-bit pushes.

这里有提到 16位 和32 位的区别，我觉得值得应该也是 TYpe最高位的含义， 还不是很清楚



```cpp
typedef struct gate_descriptor{
    uint16_t offest_low;                    
    uint16_t segment_selector;              //  这里对应的是 gdt 中放在 cs 等段寄存器中的 selector
    uint8_t  reserved;
    uint8_t  type : 4;
    uint8_t  segment : 1;
    uint8_t  DPL : 2;
    uint8_t  P : 1;
    uint16_t offest_high;
} _packed gate_descriptor;

```
1. 中断类型

内中断（软中断（int） + 异常）+ 外中断

对应 gdtr 寄存器load 和 保存的右 idtr 寄存器 和相应的结构，但是由于和gdt是一样的所以就使用的同一个

```cpp
typedef struct gdtr_content{            //  找到GDT表，lgdt指令加载到gdtr寄存器中
    uint16_t limit;
    uint32_t base_addr;
} _packed gdtr_content;
```



对应 gdt 表也有对应的 gate descriptor table

异常在我的理解就是可以不需要手动调用，而是由 CPU 自己触发的中断方式

> Exceptions are classified as faults, traps, or aborts depending on the way they are reported and whether the
instruction that caused the exception can be restarted without loss of program or task continuity.

+ Faults — A fault is an exception that can generally be corrected and that, once corrected, allows the program
to be restarted with no loss of continuity. When a fault is reported, the processor restores the machine state to
the state prior to the beginning of execution of the faulting instruction. The return address (saved contents of
the CS and EIP registers) for the fault handler points to the faulting instruction, rather than to the instruction
following the faulting instruction.

+ Traps — A trap is an exception that is reported immediately following the execution of the trapping instruction.
Traps allow execution of a program or task to be continued without loss of program continuity. The return
address for the trap handler points to the instruction to be executed after the trapping instruction.

+ Aborts — An abort is an exception that does not always report the precise location of the instruction causing
the exception and does not allow a restart of the program or task that caused the exception. Aborts are used to
report severe errors, such as hardware errors and inconsistent or illegal values in system tables.

手册指出：

> The allowable range for vector numbers is 0 to 255. Vector numbers in the range 0 through 31 are reserved by the
Intel 64 and IA-32 architectures for architecture-defined exceptions and interrupts. Not all of the vector numbers
in this range have a currently defined function. The unassigned vector numbers in this range are reserved. Do not
use the reserved vector numbers.

前32个中断是cpu 指定的，但是 只能有255 个IDT 好像似乎可以有很多


Name	                Vector  	            Type	        Mnemonic	    Error code?
Division Error	        0 (0x0)	                Fault	        #DE	            No
Debug	                1 (0x1)	                Fault/Trap	    #DB	            No
Non-maskable Interrupt	2 (0x2)	                Interrupt	-	No
Breakpoint	            3 (0x3)	                Trap	        #BP	            No
Overflow	            4 (0x4)	                Trap	        #OF	            No
Bound Range Exceeded	5 (0x5)	                Fault	        #BR	            No
Invalid Opcode	        6 (0x6)	                Fault	        #UD	            No
Device Not Available	7 (0x7)	                Fault	        #NM	            No
Double Fault	        8 (0x8)	                Abort	        #DF	            Yes (Zero)
Coprocessor Segment Overrun	9 (0x9)	            Fault	        -	            No
Invalid TSS	                10 (0xA)	        Fault	        #TS	            Yes
Segment Not Present	        11 (0xB)	        Fault	        #NP	            Yes
Stack-Segment Fault	        12 (0xC)	        Fault	        #SS	            Yes
General Protection Fault    13 (0xD)	        Fault	        #GP	            Yes
Page Fault	                14 (0xE)	        Fault	        #PF	            Yes
Reserved	                15 (0xF)	        -	            -	            No
x87 Floating-Point Exception	16 (0x10)	    Fault	        #MF	            No
Alignment Check	                17 (0x11)	    Fault	        #AC	            Yes
Machine Check	                18 (0x12)	    Abort	        #MC	            No
SIMD Floating-Point Exception	19 (0x13)	    Fault	        #XM/#XF	        No
Virtualization Exception	    20 (0x14)	    Fault	        #VE 	        No
Control Protection Exception	21 (0x15)	    Fault	        #CP	            Yes
Reserved	                    22-27 (0x16-0x1B)	-	        -	            No
Hypervisor Injection Exception	28 (0x1C)	    Fault	        #HV	            No
VMM Communication Exception	    29 (0x1D)	    Fault	        #VC	            Yes
Security Exception	            30 (0x1E)	    Fault	        #SX	            Yes
Reserved	                    31 (0x1F)	    -	-	        No


所以说 ， 那些 Error code 是 yes 的， 也就是在异常的时候cpu 会自动压入一个 错误码的吗？

    + 测试了一下，果然 int 一个没有的中断号， 据up 说是会触发 一般性保护异常，虽然不确定，但是栈中确实多压入了4个字节，我这里是 0x0402
    
    + 最开始没有把 错误的 vector 给传出来， 传出来之后就可以在 exception_handler 根据参数 打印不同的信息，可以发现 确实是 General Protection Fault


nasm [宏](https://www.nasm.us/xdoc/2.15.05/html/nasmdoc4.html#section-4.3)

具体的与处理结果 可以用 nasm -E 来查看

手册里也可以看到，Error code 的结构也是有含义的，具体先不了解