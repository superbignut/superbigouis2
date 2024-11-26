[bios](https://wiki.osdev.org/BIOS)
> BIOS (Basic Input/Output System) was created to offer generalized low-level services to early PC system programmers. The basic aims were: to hide (as much as possible) variations in PC models and hardware from the OS and applications, and to make OS and application development easier (because the BIOS services handled most of the hardware level interface).

> These BIOS services are still used (especially during bootup), and are often named "BIOS functions". In Real Mode, they can be easily accessed through software interrupts, using Assembly language.

int 0x10 这种就都是 bios的函数


[主引导扇区]

功能：读取bootloader 并执行


[int 0x10中断参考](https://stanislavs.org/helppc/int_10-0.html)

+ int 0x10 整体是设置显示的模式，我们使用的是 ax = 0x03, 具体暂不深入了解
    AH = 00
	AL = 00  40x25 B/W text (CGA,EGA,MCGA,VGA)
	   = 01  40x25 16 color text (CGA,EGA,MCGA,VGA)
	   = 02  80x25 16 shades of gray text (CGA,EGA,MCGA,VGA)
	   = 03  80x25 16 color text (CGA,EGA,MCGA,VGA)

调用bios函数，目前来说起到的作用就是 实现了清屏的功能, 但我记得这就是一个jmp函数

> The INT n instruction generates a call to the interrupt or exception handler specified with the destination operand(see the section titled “Interrupts and Exceptions” in Chapter 6 of the Intel ® 64 and IA-32 Architectures SoftwareDeveloper’s Manual, Volume 1). The destination operand specifies a vector from 0 to 255, encoded as an 8-bitunsigned intermediate value. Each vector provides an index to a gate descriptor in the IDT. The first 32 vectors arereserved by Intel for system use. Some of these vectors are used for internally generated exceptions.

+ INT 0x10, AH = 0xE -- display char 

把字符放到AL中，进而调用中断， 就可以将字符打印到屏幕上， 具体的bios介绍，可见up给的参考书，在osdev中的最后也有提及，是ibm的参考手册：

> IBM PS 2 and PC BIOS Interface Technical Reference 

并且看到，说bios 不能在保护模式中使用：

> Unfortunately, in Protected mode, almost all BIOS functions become unavailable, and trying to call them nonetheless will result in exceptions or unreliable responses (because of the different way segment values are handled). Some newer services however (such as SMBios, PCI, PnP, or VBE) offer an interface that is compatible with 32bit Protected Mode.

> If you must use Real Mode BIOS functions after the CPU has been switched into Protected Mode, then see Virtual 8086 Mode, or perhaps exit Protected Mode, and momentarily return to Real Mode. Both methods have serious problems, and therefore any calls to the BIOS should be done before any physical device is programmed by your code: