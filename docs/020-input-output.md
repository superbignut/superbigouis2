+ 输入和输出

[VGA](https://wiki.osdev.org/VGA_Hardware)

一看到 vga 的时候还没有意识到，这个接口就是那个 深蓝色的、好多空的显示器线，并且两端还有两个 特别难扭的螺丝（如果和显示器背面贴的太近的话）

> Even though VGA is old, many modern graphics cards are compatible with it, including NVidia and ATI cards. This can make writing a VGA driver rather attractive. The amount of compatibility varies however, so never assume a card is compatible without proper hardware detection. Apart from real machines, several emulators and virtual machines provide VGA emulation: Bochs, QEMU and Microsoft Virtual PC to name a few. After 2011 video card manufacturers begun to drop VGA compatibility in favour of GOP under UEFI (VirtualBox UEFI and TianoCore both supports that).

osdev 提到 vga 正在被逐渐淘汰了，但是在虚拟机中使用应该还是ok的

> While the VGA chip is quite a simple piece of hardware compared to modern video equipment, it is possibly one of the more complicated devices to program for, and especially in the old days knowing your way around this particular device was sufficient for establishing quite a reputation. 

reputation 是真堵加堵

> The VGA has a lot (over 300!) internal registers, while occupying only a short range in the I/O address space. To cope, many registers are indexed. This means that you fill one field with the number of the register to write, and then read or write another field to get/set the actual register's value.

VGA的寄存器太多 所以需要一种间接寻址的方式进行读写


> All registers are accessed as 8 bit. The parts of a register that are not used should officially be preserved, although a lot of programs simply set them to zero. However, not all fields present in the VGA are documented here, so you will either look up a different reference, or preserve the undocumented fields.

寄存器都是8位的

主要参考链接： [CRT](http://www.osdever.net/FreeVGA/vga/crtcreg.htm#0E)

VGA相当于是提供了一个规范，或者是一些标准，但是具体在操作光标的时候，控制的还是 CRT-Register (阴极射线管寄存器)


[FreeVGA](http://www.osdever.net/FreeVGA/home.htm)

CRT 地址寄存器 0x3D4
CRT 数据寄存器 0x3D5
CRT 光标位置 高位 0xE
CRT 光标位置 低位 0xF



[Introduction to Low-level Programming ](http://www.osdever.net/FreeVGA/llintro.htm)
> How do memory and I/O ports work?

>80x86 machines have both a memory address space and an input/output (I/O) address space. Most of the memory is provided as system RAM on the motherboard and most of the I/O devices are provided by cards (although the motherboard does provide quite a bit of I/O capability, varying on the motherboard design.) Also some cards also provide memory. The VGA and SVGA display adapters provide memory in the form of video memory, and they also handle I/O addresses for controlling the display, so you must learn to deal with both. An adapter card could perform all of its functions using solely memory or I/O (and some do), but I/O is usually used because the decoding circuitry is simpler and memory is used when higher performance is required.

>The original PC design was based upon the capabilities of the 8086/8088, which allowed for only 1 MB of memory, of which a small range (64K) was allotted for graphics memory. Designers of high-resolution video cards needed to put more than 64K of memory on their video adapters to support higher resolution modes, and used a concept called "banking" which made the 64K available to the processor into a "window" which shows a 64K chunk of video memory at once. Later designs used multiple banks and other techniques to simplify programming. Since modern 32-bit processors have 4 gigabytes of address space, some designers allow you to map all of the video memory into a "linear frame buffer" allowing access to the entire video memory at once without having to change the current window pointer (which can be time consuming.) while still providing support for the windowed method.

> Memory can be accessed most flexibly as it can be the source and/or target of almost every machine language instruction the CPU is capable of executing, as opposed to a very limited set of I/O instructions. I/O space is divided into 65536 addresses in the range 0-65535. Most I/O devices are configured to use a limited set of addresses that cannot conflict with another device. The primary instructions for accessing I/O are the assembly instructions "IN" and "OUT", simply enough. Most programming environments provide similarly named instructions, functions, or procedures for accessing these.

介绍中大致说了一下显卡，还有最开始的64k显存

[Overview of Video Hardware Functionality](http://www.osdever.net/FreeVGA/hardovr.htm)

> The basic function of graphics hardware is to allow the CPU to manipulate memory specific to the graphics hardware, and to take the information stored in that memory and output it in a form that a monitor or LCD panel can use.

介绍了 Frame Buffer， Graphics Controller， 感觉写的挺明白的，所以说显存并不是在内存中的吗，只是映射过去的吗？




其实最奇怪的地方就是 VGA 的使用方法， 
+ 读：
    1. 先把想要读的地址 放到 addr 寄存器
    2. 去 data 寄存器拿出数据
+ 写:
    1. 先把想要写的地址 放到 addr 寄存器
    2. 把要写的数据写入 data 寄存器
