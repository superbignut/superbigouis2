外中断的话，在虚拟机里应该默认就是操作 8259A PIC programmable interrupt controllerr

> The 8259 Programmable Interrupt Controller (PIC) is one of the most important chips making up the x86 architecture. Without it, the x86 architecture would not be an interrupt driven architecture. The function of the 8259A is to manage hardware interrupts and send them to the appropriate system interrupt. This allows the system to respond to devices needs without loss of time (from polling the device, for instance).

> It is important to note that APIC has replaced the 8259 PIC in more modern systems, especially those with multiple cores/processors.

主要参考8259[手册](https://pdos.csail.mit.edu/6.828/2005/readings/hardware/8259A.pdf)


因为要大致捋清楚中断的触发和响应的流程，因此对 8259 的寄存器总是要了解一下：


手册首先介绍了 轮询 polled 和 interrupt 的区别。

> Each peripheral device or structure usually has a special program or ‘‘routine’’ that is associated with its specific functional or operational requirements; this is referred to as a ‘‘service routine’’. The PIC, after issuing an Interrupt to the CPU, must somehow input information into the CPU that can ‘‘point’’ the Program Counter to the service routine associated with the requesting device. This ‘‘pointer’’ is an address in a vectoring table and will often be referred to, in this document, as vectoring data.

这段话的意思是，每一个终端设备还有一个自己的 中断程序，每当 PIC 发起一个中断给cpu之后，PIC 还需要紧接着把 中断向量 给到 cpu ，


手册上首页的框图给出了8259主要的结构：
    + ISR  In Service Reg - 存储cpu正在服务的中断
    + Priority Resolver - 将 IRR 的最优先的中断传入 ISR
    + IRR  Interrupt Request Reg - 并带有IR0~IR7 共8个输入引脚，连接各中断设备， 表示有那些中断进入
    + IMR  Inrerrupt Mask Reg - 屏蔽字，屏蔽高优先级 不会影响 低优先级
    + Control Logic - 输入引脚INTA用来让CPU获取中断向量，输出引脚INT用于向CPU触发中断
      + 接收到来自cpu 的指令
      + ICW 寄存器
      + OCW 寄存器
    + Data bus buffer - D0~D7 用于控制、状态、中断向量的数据的传输
    + Read/Write Logic - 包括用于初始化的ICW寄存器s，用于控制的OCW寄存器s
    + Cascade Buffer - 级联功能


> The powerful features of the 8259A in a microcomputer system are its programmability and the interrupt routine addressing capability.

8259A 的两大特点是：
    + 可编程能力
    + 可寻址（中断程序）

1. One or more of the INTERRUPT REQUEST lines (IR7 –0) are raised high, setting the corresponding IRR bit(s).
   1. 首先， 中断到来
2. The 8259A evaluates these requests, and sends an INT to the CPU, if appropriate.
   1. 8259A 把中断 传递给 cpu
3. The CPU acknowledges the INT and responds with an INTA pulse.
   1. CPU 下拉 INTA 信号， 表示接收 中断
4. Upon receiving an INTA from the CPU group, the highest priority ISR bit is set and the corresponding IRR bit is reset. The 8259A does not drive the Data Bus during this cycle.
   1. CPU 准备选择最高优先级的中断去执行， ISR 置 1 对应的 IRR 复位
5. The 8086 will initiate a second INTA pulse. During this pulse, the 8259A releases an 8-bit pointer onto the Data Bus where it is read by the CPU.
   1. cpu 会触发在 下一次 INTA 信号的时候 拿到中断向量
6. This completes the interrupt cycle. In the AEOI mode the ISR bit is reset at the end of the second INTA pulse. Otherwise, the ISR bit remains set until an appropriate EOI command is issuedat the end of the interrupt subroutine.
   1. 如果是手动结束中断，则需要 调用 EOI 指令

> In protected mode, the IRQs 0 to 7 conflict with the CPU exception which are reserved by Intel up until 0x1F. (It was an IBM design mistake.) Consequently it is difficult to tell the difference between an IRQ or an software error. It is thus recommended to change the PIC's offsets (also known as remapping the PIC) so that IRQs use non-reserved vectors. A common choice is to move them to the beginning of the available range (IRQs 0..0xF -> INT 0x20..0x2F). For that, we need to set the master PIC's offset to 0x20 and the slave's to 0x28. For code examples, see below

这里好像是 说 IRQ的中断号 0-7  会和CPU 异常中断号冲突，因此需要移动一下 一般是使用 0x20 开始的中断号