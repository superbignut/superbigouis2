[c-string-cppreference](https://en.cppreference.com/w/c/string/byte)

暂时的话， 加了一些函数进去，但之前没有去想，现在很多c的语法，都想着去看看汇编是怎么实现的， 比如：

+ 函数返回类型不匹配， 是怎么转换的


```c

int add(int x, int y){
    double z = 1.0;
    return z;
}
int main(){

    int a = 1;
    int b = 2;
    int c  = add(a, b);

    return 0;
}
```

我这里尝试了一下 将简单的double 转为int 然后返回，但是汇编看起来有点奇怪：

```asm
_add:
	pushl	%ebp
	movl	%esp, %ebp          ;  虽然保存了栈帧，但是后面没有使用
	andl	$-8, %esp           ;  这里不太明白， 只是为了对齐操作吗 0xfff8, 后三位置0
	subl	$16, %esp           ;  这里也发现没有用后面的esp 偏移 0 的四个字节， 只用到了偏移8和偏移4
	fld1
	fstpl	8(%esp)
	fldl	8(%esp)
	fisttpl	4(%esp)
	movl	4(%esp), %eax
	leave
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
```


---


+ 022 基础显卡驱动

[Mapping of Display Memory into CPU Address Space](http://www.osdever.net/FreeVGA/vga/vgamem.htm)

从这里看到，原来0xb8000的设定是在显卡的寄存器中指定的，感觉还挺奇怪的，我还以为这个配置应该是cpu来指定的，那么在与 int 0x10 这个中断结合起来的话，

应该就是 这个bios 中断 修改了 VGA 的寄存器，进而进入了80*25的文本显示模式




[FreeVGA](http://www.osdever.net/FreeVGA/home.htm)

CRT 地址寄存器 0x3D4
CRT 数据寄存器 0x3D5
CRT 光标位置 高位 0xE
CRT 光标位置 低位 0xF

这里在输入输出的基础上 增加了

CRT 显示开始的位置 高位 0xC
CRT 显示开始的位置 低位 0xD

> This contains the bits 7-0 of the Start Address field. The upper 8 bits are specified by the Start Address High Register. The Start Address field specifies the display memory address of the upper left pixel or character of the screen. Because the standard VGA has a maximum of 256K of memory, and memory is accessed 32 bits at a time, this 16-bit field is sufficient to allow the screen to start at any memory address. Normally this field is programmed to 0h, except when using virtual resolutions, paging, and/or split-screen operation. Note that the VGA display will wrap around in display memory if the starting address is too high. (This may or may not be desirable, depending on your intentions.)

指的是VGA支持很大的显存，因此，不一定要使用0作为 起始地址，因此使用16位来给开始显示的位置进行定位

> Memory Map Select
This field specifies the range of host memory addresses that is decoded by the VGA hardware and mapped into display memory accesses.  The values of this field and their corresponding host memory ranges are:
	00b -- A0000h-BFFFFh (128K region)
	01b -- A0000h-AFFFFh (64K region)
	10b -- B0000h-B7FFFh (32K region)
	11b -- B8000h-BFFFFh (32K region)

这里具体是去读VGA的 Memory Map Select 位， 我读出来的是 14（0b0000_1110） 所以我的地址起始 0xB8000 大小是 32K=0x 8000