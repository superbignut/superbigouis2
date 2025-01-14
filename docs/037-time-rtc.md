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