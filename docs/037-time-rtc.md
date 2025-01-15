> "CMOS" is a tiny bit of very low power static memory that lives on the same chip as the Real-Time Clock (RTC). It was introduced to IBM PC AT in 1984 which used Motorola MC146818A RTC.

> CMOS (and the Real-Time Clock) can only be accessed through IO Ports 0x70 and 0x71. The function of the CMOS memory is to store 50 (or 114) bytes of "Setup" information for the BIOS while the computer is turned off -- because there is a separate battery that keeps the Clock and the CMOS information active.

> CMOS values are accessed a byte at a time, and each byte is individually addressable. Each CMOS address is traditionally called a "register". The first 14 CMOS registers access and control the Real-Time Clock. In fact, the only truly useful registers remaining in CMOS are the Real-Time Clock registers, and register 0x10. All other registers in CMOS are almost entirely obsolete (or are not standardized), and are therefore useless.

cmos 我一直记得是模电中的一种三极管，我不确定这里为什么也用这个 名字来指代 静态存储器


> Whenever you send a byte to IO port 0x70, the high order bit tells the hardware whether to disable NMIs from reaching the CPU. If the bit is on, NMI is disabled (until the next time you send a byte to Port 0x70). The low order 7 bits of any byte sent to Port 0x70 are used to address CMOS registers.

写入 cmos 时，还会影响 NMI 可屏蔽中断


Whenever you send a byte to IO port 0x70, the high order bit tells the hardware whether to disable NMIs from reaching the CPU. If the bit is on, NMI is disabled (until the next time you send a byte to Port 0x70). The low order 7 bits of any byte sent to Port 0x70 are used to address CMOS registers.

        outb (0x70, (NMI_disable_bit << 7) | (selected CMOS register number));

Once a register is selected, you either read the value of that register on Port 0x71 (with inb or an equivalent function), or you write a new value to that register -- also on Port 0x71 (with outb, for example):

        val_8bit = inb (0x71);

因此每次写入的时候，还要顺带着把 NMI 给连带屏蔽了


The format of the ten clock data registers (bytes 00h-09h) is:

 00h Seconds       (BCD 00-59, Hex 00-3B) Note: Bit 7 is read only
 01h Second Alarm  (BCD 00-59, Hex 00-3B; "don't care" if C0-FF)
 02h Minutes       (BCD 00-59, Hex 00-3B)
 03h Minute Alarm  (BCD 00-59, Hex 00-3B; "don't care" if C0-FF)
 04h Hours         (BCD 00-23, Hex 00-17 if 24 hr mode)
						 (BCD 01-12, Hex 01-0C if 12 hr am)
						 (BCD 81-92. Hex 81-8C if 12 hr pm)
 05h Hour Alarm    (same as hours; "don't care" if C0-FF)
 06h Day of Week   (01-07 Sunday=1)
 07h Date of Month (BCD 01-31, Hex 01-1F)
 08h Month         (BCD 01-12, Hex 01-0C)
 09h Year          (BCD 00-99, Hex 00-63)

 [ref](https://web.archive.org/web/20111209041013/http://www-ivs.cs.uni-magdeburg.de/~zbrog/asm/cmos.html)


 [MC146818A](https://web.stanford.edu/class/cs140/projects/pintos/specs/mc146818a.pdf) 这里面给了一个例子，所以貌似 CMOS 返回的 year 是从1900 开始的，而不是 1970

 但是由于 现在的很多 时间戳比如 python的time 都是 从1970 开始的，所以又要 找齐1970，就挺麻烦的，

 最后的 qemu 里面的时区是 要慢 8个小时，暂时不知道怎么解决， bochs 时间是对的



---
#### rtc

实时时钟中断， 这里的 CMOS 中也可以触发一个中断，但是要和 8253 的那个时钟中断区分开，那个是 irq0， rtc 这个是 irq8

这个先不写了，以后用到再写吧！

> The RTC also can generate clock ticks on IRQ8 (similarly to what the PIT does on IRQ0). The highest feasible clock frequency is 8KHz. Using the RTC clock this way may actually generate more stable clock pulses than the PIT can generate. It also frees up the PIT for timing events that really need near-microsecond accuracy. Additionally, the RTC can generate an IRQ8 at a particular time of day. See the RTC article for more detailed information about using RTC interrupts.


这里的 osdev 介绍的并不全，所以还是要去原文档看比较准确：

文档中的 address map 给出了 mc146818 一共有 64 个字节的存储RAM，前14 个用来做日期存储，有单独供电所以不会丢失信息，后50就没有了


 #### Status Register A (read/write)
  Bit 7     - (1) time update cycle in progress, data ouputs undefined
				  (bit 7 is read only)
  Bit 6,5,4 - 22 stage divider. 010b - 32.768 Khz time base (default)
        000： 4.194302MHZ
        001： 1.048576MHZ
        010： 32.768MHZ
        这三位就是用来选择一个基础频率的
  Bit 3-0   - Rate selection bits for interrupt.
				  0000b - none
				  0011b - 122 microseconds (minimum)
				  1111b - 500 milliseconds
				  0110b - 976.562 microseconds (default)
        这三位是用来做一个分频的
        
        "copy from onix", 如果使用32.768MHZ 作为基础频率，分频效果如下，时间片也就是频率的倒数
        | 3 ~ 0 位 | 时间片       |
        | -------- | ------------ |
        | 0000     | 从不触发中断 |
        | 0001     | 3.90625 ms   |
        | 0010     | 7.8125 ms    |
        | 0011     | 122.070 μs   |
        | 0100     | 244.141 μs   |
        | 0101     | 488.281 μs   |
        | 0110     | 976.5625 μs  |
        | 0111     | 1.953125 ms  |
        | 1000     | 3.90625 ms   |
        | 1001     | 7.8125 ms    |
        | 1010     | 5.625 ms     |
        | 1011     | 1.25 ms      |
        | 1100     | 62.5 ms      |
        | 1101     | 125 ms       |
        | 1110     | 250 ms       |
        | 1111     | 500 ms       |


 #### Status Register B (read/write)
  Bit 7 - 1 enables cycle update, 0 disables
  Bit 6 - 1 enables periodic interrupt
  Bit 5 - 1 enables alarm interrupt
  Bit 4 - 1 enables update-ended interrupt
  Bit 3 - 1 enables square wave output
  Bit 2 - Data Mode - 0: BCD, 1: Binary
  Bit 1 - 24/12 hour selection - 1 enables 24 hour mode
  Bit 0 - Daylight Savings Enable - 1 enables

 #### 0Ch Status Register C (Read only)
  Bit 7 - Interrupt request flag - 1 when any or all of bits 6-4 are
			 1 and appropriate enables (Register B) are set to 1. Generates
			 IRQ 8 when triggered.
  Bit 6 - Periodic Interrupt flag
  Bit 5 - Alarm Interrupt flag
  Bit 4 - Update-Ended Interrupt Flag
  Bit 3-0 ???

 0Dh Status Register D (read only)
  Bit 7 - Valid RAM - 1 indicates batery power good, 0 if dead or
			 disconnected.
  Bit 6-0 ???

  + rtc 支持三种中断方式，体现在 B 寄存器的456三位上， 每当这三个中断产生后，在 C 寄存器中就会有 对应的456三个标志位被置位。
    + 其实那个方波中断 感觉和 8253 的是一样的
    + 闹钟中断似乎特别一点
    + 更新结束中断应该用不到
  + C 寄存器每次读取后会被 clear ，所以如果两位 都被需要的话，要一次读出来
  + C 的 最高位 是 IRQF 用来标志 是否有被允许的中断 已经 触发
