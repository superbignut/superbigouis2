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


> When you enter protected mode (or even before hand, if you're not using GRUB) the first command you will need to give the two PICs is the initialise command (code 0x11). This command makes the PIC wait for 3 extra "initialisation words" on the data port. These bytes give the PIC:

+ Its vector offset. (ICW2)
+ Tell it how it is wired to master/slaves. (ICW3)
+ Gives additional information about the environment. (ICW4)

这个 PIC 初始化的方式也是 挺奇怪的， 看 手册上的图 大致能明白一点

我的理解就是，ICW1 和 ICW2 可以拼成 16位  然后相当于是 做一些配置给 8259


#### ICW1

| 7   | 6   | 5   | 4   | 3    | 2   | 1     | 0   |
| --- | --- | --- | --- | ---- | --- | ----- | --- |
| 0   | 0   | 0   | 1   | LTIM | ADI | SINGL | IC4 |

+ IC4 0 表示 不使用 ICW4
+ SINGLE 0 表示 使用 ICW3 也是多片的意思 1表示 只有一个
+ ADI 用作 MCS 80/85 表示时间间隔 8086不用
+ LTIM 1 表示电平模式


#### ICW2

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| T7  | T6  | T5  | T4  | T3  | ID2 | ID1 | ID0 |

+ 应该只有 3-7位 被表示成 向量号

```cpp
// copy from [github](https://github.com/StevenBaby/onix/commit/7202bce4e3342ebce65eae22a1adc5905165342e#diff-939a6d9371300f384b89f1b70d6f828cc3021006e8b83cc2d3e4dc5d6cb85332R77)
由于每个 8259A 芯片上的 IRQ 接口是顺序排列的，所以咱们这里的设置就是指定 IRQ0 映射到的中断向量号，其他 IRQ 接口对应的中断向量号会顺着自动排列下去。

ICW2 需要写入到主片的 `0x21` 端口和从片的 `0xA1`；

由于咱们只需要设置 IRQ0 的中断向量号， IRQ1~IRQ7 的中断向量号是 IRQ0 的顺延，所以，只需填写高 5 位 T3~T7，ID0~ID2 这低 3 位不用填。

由于只填写高 5 位，所以任意数字都是 8 的倍数，这个数字表示的便是设定的起始中断向量号。

这是有意设计的，低 3 位能表示 8 个中断向量号，这由 8259A 根据 8 个 IRQ 接口的排列位次自行导入，IRQ0 的值是 000, IRQ1 的值是001, IRQ2 的值便是 010 ... 以此类推，这样高 5 位加 低 3 位，便表示了任意一个 IRQ 接口实际分配的中断向量号。
n
```

> If no interrupt request is present at step 4 of either sequence (i.e., the request was too short in duration) the 8259A will issue an interrupt level 7. Both the vectoring bytes and the CAS lines will look like an interrupt level 7 was requested.

> In an 8086 system A15–A11 are inserted in the five most significant bits of the vectoring byte and the 8259A sets the three least significant bits according to the interrupt level.



#### ICW3 

主片中 哪一位置1 表示哪一位 有从片

从片中 后三位表示 ID 用于 标记 连在了 哪个 主片的IRQ上


#### ICW4

| 7   | 6   | 5   | 4    | 3   | 2   | 1    | 0   |
| --- | --- | --- | ---- | --- | --- | ---- | --- |
| 0   | 0   | 0   | SFNM | BUF | M/S | AEOI | μPM |

PM 1 表示 8086
AEOI 1 表示 自动结束
M/S 
BUF 1 表示 工作在 缓冲模式？
SFNM 嵌套？




#### OCW1: copy from onix doc

> Each Interrupt Request input can bem masked individually by the Interrupt Mask Register (IMR) programmed through OCW1. 

```cpp

   OCW1 用来屏蔽连接在 8259A 上的外部设备的中断信号，如下表所示，共一个字节；

   | 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
   | --- | --- | --- | --- | --- | --- | --- | --- |
   | M7  | M6  | M5  | M4  | M3  | M2  | M1  | M0  |

   实际上就是把 OCW1 写入了即 IMR 寄存器。这里的屏蔽是说是否把来自外部设备的中断信号转发给 CPU。由于外部设备的中断都是可屏蔽中断，所以最终还是要受标志寄存器 eflags 中的 IF 位的管束，若 IF 为 0，可屏蔽中断全部被屏蔽，也就是说，在 IF 为 0 的情况下，即使 8259A 把外部设备的中断向量号发过来，CPU 也置之不理。
   OCW1 要写入主片的 `0x21` 或从片的 `0xA1` 端口；

```



基本大多数的问题 在doc 中都有提到：[QA](https://github.com/StevenBaby/onix/commit/7202bce4e3342ebce65eae22a1adc5905165342e#diff-939a6d9371300f384b89f1b70d6f828cc3021006e8b83cc2d3e4dc5d6cb85332R265)





总体下来，几乎全部的流程就是在配置 8259，其中最核心的操作 就是设置了外部中断 主片和从片的 中断号
   1. ICW 配置中断号
   2. OCW 设置屏蔽字
   3. 设置中断处理函数
   4. sti 开中断
   5. 每次中断 手动 EOI
  