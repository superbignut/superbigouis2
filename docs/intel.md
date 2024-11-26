
[volume1-6.5]()

> The processor provides two mechanisms for interrupting program execution, interrupts, and exceptions:
> • An interrupt is an asynchronous event that is typically triggered by an I/O device.
> • An exception is a synchronous event that is generated when the processor detects one or more predefinedconditions while executing an instruction. TheIA-32 architecture specifies three classes of exceptions: faults,traps, and aborts.4

> The processor responds to interrupts and exceptions in essentially the same way. When an interrupt or exceptionis signaled, the processor halts execution of the current program or task and switches to a handler procedure thathas been written specifically to handle the interrupt or exception condition. The processor accesses the handlerprocedure through an entry in the interrupt descriptor table (IDT). When the handler has completed handling theinterrupt or exception, program control is returned to the interrupted program or task.

int中断的话按照这里来理解就是 去IDT里面找处理函数，然后再调回来，确实和jmp很像哦，但是更像是一个全局函数


> The IA-32 Architecture defines 18 predefined interrupts and exceptions and 224 user defined interrupts, which areassociated with entries in the IDT. Each interrupt and exception in the IDT is identified with a number, called a vector. Table 6-1 lists the interrupts and exceptions with entries in the IDT and their respective vectors. Vectors 0through 8, 10 through 14, and 16 through 19 are the predefined interrupts and exceptions; vectors 32 through 255 are for software-defined interrupts, which are for either software interrupts or maskable hardware inter-rupts.

IA-32 前面的中断被称为中断和异常， 后面的中断被称为 软件中断和硬件可屏蔽中断

[int 中断]()

> If no stack switch occurs, the processor does the following when calling an interrupt or exception handler (seeFigure 6-7):
    1. Pushes the current contents of the EFLAGS, CS, and EIP registers (in that order) on the stack.
    If shadow stack is enabled:
        a. Temporarily saves the current value of the SSP register internally.
        b. Pushes the current value of the CS register on the shadow stack.
        c. Pushes the current value of LIP (CS.base + EIP) on the shadow stack.
        d. Pushes the temporarily saved SSP value on the shadow stack.
    2. Pushes an error code (if appropriate) on the stack.
    3. Loads the segment selector for the new code segment and the new instruction pointer (from the interrupt gate
    or trap gate) into the CS and EIP registers, respectively.
    4. If the call is through an interrupt gate, clears the IF flag in the EFLAGS register.
    5. Begins execution of the handler procedure.

就是说要将eflags, cs, ip 入栈，然后 加载新的 cs eip 

[V1-6.2stack]()
> The stack (see Figure 6-1) is a contiguous array of memory locations. It is contained in a segment and identified by
the segment selector in the SS register. When using the flat memory model, the stack can be located anywhere in
the linear address space for the program. A stack can be up to 4 GBytes long, the maximum size of a segment.
Items are placed on the stack using the PUSH instruction and removed from the stack using the POP instruction.
When an item is pushed onto the stack, the processor decrements the ESP register, then writes the item at the new
top of stack. When an item is popped off the stack, the processor reads the item from the top of stack, then incre-
ments the ESP register. In this manner, the stack grows down in memory (towards lesser addresses) when items

push的时候 栈的地址向小地址方向增加

[osdev-IVT](https://wiki.osdev.org/Interrupt_Vector_Table)

> On the x86 architecture, the Interrupt Vector Table (IVT) is a table that specifies the addresses of all the 256 interrupt handlers used in real mode.
The IVT is typically located at 0000:0000H, and is 400H bytes in size (4 bytes for each interrupt). Although the default address can be changed using the LIDT instruction on newer CPUs, this is usually not done because it is both inconvenient and incompatible with other implementations and/or older software (e.g. MS-DOS programs). However, note that the code must remain in the first MiB of RAM.

这里查IDT会和保护模式下的概念冲突，ivt才是对的，使用做 int 0x** 中断用的, 比较特别的是，低地址是ip 高地址是段寄存器，所以这是要跳转到cs:ip的位置继续执行指令喽

            mov word [0x80 * 4], print  ;ip
            mov word [0x80 * 4 + 2], 0  ;cs
            int 0x80                    ;调用自定义中断

[V1-6.4]()这章有对
    + Near call and ret
    + Far  call and ret
进行讨论

[V1-6.5]()则是对中断int 的行为进行了说明

再加上[V1-IRET]()对iret 等指令的说明，

总结如下：

使用call 和call far 时，分别会将ip和cs、ip入栈，而在 int 的时候则是将 cs、ip、EFLAG三个入栈;对应的也就需要ret、retf和iret分别将对应数量的寄存器出栈。

        + call / ret -> ip
        + callf / retf -> ip + cs 
        + int -> ip + cs + eflag


下面是iret的测试代码，使用bochs debug可以观察栈的变化：
                
            ; 注册中断向量
            mov word [0x80 * 4], print  ;ip
            mov word [0x80 * 4 + 2], 0  ;cs

            ; 调用中断
            int 0x80    
            jmp $

        print:
            mov ax, 0
            mov ds, ax
            mov ax, 0xb800
            mov ds, ax
            mov byte [ds:0], 'H'
            iret

            times 510 - ($-$$) db 0
            db 0x55, 0xaa


jmp 和 jmpf 的区别是什么