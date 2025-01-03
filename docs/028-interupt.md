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
