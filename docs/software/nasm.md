[nasm 编译器语法](https://www.nasm.us/xdoc/2.16.02/html/nasmdoc0.html)

在写最简单的第一个在屏幕上显示字母的汇编程序时，用到了很多奇怪的语法: times, db, $ ,$$之类，在查找NASM官网时看到[Chapter 3: The NASM Language](https://nasm.us/doc/nasmdoc3.html)对这些语法进行了解释，比如：

> NASM supports two special tokens in expressions, allowing calculations to involve the current assembly position: the $ and $$ tokens. $ evaluates to the assembly position at the beginning of the line containing the expression; so you can code an infinite loop using JMP $. $$ evaluates to the beginning of the current section; so you can tell how far into the section you are by using ($ - $$).

> The TIMES prefix causes the instruction to be assembled multiple times

[nasm:times]() 命令这么说的话就是重复多遍汇编指令

        label:    instruction operands        ; comment

> NASM places no restrictions on white space within a line: labels may have white space before them, or instructions may have no space before them, or anything. The colon after a label is also optional. 

[nasm:org]() 是用来指定汇编的初始地址的，也就是说这是一个编译器的假设，假设程序的首地址，因此当在程序中涉及到具体的地址的时候，这个假设的偏移量都会被加上去。

+ ORG : [Binary File Program Origin](https://www.nasm.us/xdoc/2.16.02/html/nasmdoc8.html)

> The function of the ORG directive is to specify the origin address which NASM will assume the program begins at when it is loaded into memory.

[nasm:equ]() 用来定义一个常数

> EQU defines a symbol to a given constant value: when EQU is used, the source line must contain a label. The action of EQU is to define the given label name to the value of its (only) operand. This definition is absolute, and cannot change later.

        message         db      'hello, world' 
        msglen          equ     $-message


[nasm : section](https://www.nasm.us/xdoc/2.13.02rc3/html/nasmdoc7.html)

section 的作用似乎就是进行对齐操作，并且对于不同的输出格式，也有不同的意义：

+ 7.1.2 bin Extensions to the SECTION Directive

> The bin output format extends the SECTION (or SEGMENT) directive to allow you to specify the alignment requirements of segments. This is done by appending the ALIGN qualifier to the end of the section-definition line. For example,

+ 7.9.2 elf extensions to the SECTION Directive

> Like the obj format, elf allows you to specify additional information on the SECTION directive line, to control the type and properties of sections you declare. Section types and properties are generated automatically by NASM for the standard section names, but may still be overridden by these qualifiers. 

+ section .text    progbits  alloc   exec    nowrite  align=16 

+ section .data    progbits  alloc   noexec  write    align=4 

+ section .bss     nobits    alloc   noexec  write    align=4 

可以看到 elf 格式与 bin 格式相比，还多了属性上的约束,但是还不确定这些约束，比如可写、不可写是在编译阶段检查的吗？还是执行阶段？这里可以尝试写一下 section .text

尝试了一下，如果写只读区域，会导致 core dump 的 error出现，所以猜测应该是将只读的section 放到特定的内存上，每当写只读区域的时候就会触发报错。

[nasm : extern](https://www.nasm.us/xdoc/2.11.08/html/nasmdoc6.html)

> EXTERN is similar to the MASM directive EXTRN and the C keyword extern: it is used to declare a symbol which is not defined anywhere in the module being assembled, but is assumed to be defined in some other module and needs to be referred to by this one. Not every object-file format can support external variables: the bin format cannot.

[nasm : global](https://www.nasm.us/xdoc/2.11.08/html/nasmdoc6.html)

> GLOBAL is the other end of EXTERN: if one module declares a symbol as EXTERN and refers to it, then in order to prevent linker errors, some other module must actually define the symbol and declare it as GLOBAL. Some assemblers use the name PUBLIC for this purpose.The GLOBAL directive applying to a symbol must appear before the definition of the symbol.

extern 和 global 需要结合使用，任何使用extern的地方都需要找到，定义它为global的地方，进而进行链接操作


[nasm: bits](https://www.nasm.us/xdoc/2.13.03/html/nasmdoc6.html#section-6.1)

>The most likely reason for using the BITS directive is to write 32-bit or 64-bit code in a flat binary file; this is because the bin output format defaults to 16-bit mode in anticipation of it being used most frequently to write DOS .COM programs, DOS .SYS device drivers and boot loader software.

这里自己尝试了一下， 四种情况全都使用

                nasm test.asm -o test.bin ； 进行编译， 结果如下：
---

        mov bx, 0x10

1. 编译出的是 BB 10 00 ， BB应该是代表 mov bx， ** ，而1000  代表的就是16位的0x10 
        
---
        mov ebx, 0x10

2. 编译出的是 66 BB 10 00 00 00 ， 66 BB代表 在16位 模式下使用 32位 的 mov ebx， ** ，而10 00 00 00 代表的就是32位的0x10 
---
        [bits 32]
        mov bx, 0x10

3. 编译出的是 66 BB 10 00, 66 在这里就是反过来的表示，为在32位模式下使用16位寄存器， 10 00 代表 16位0x10
---
        [bits 32]
        mov ebx, 0x10

4. 编译出的是 BB 10 00 00 00 , 可以看到， 1和4的情况是没有66 的也就是 对应处理器位数相匹配的情况

---

因此，在cpu 是保护模式的时候，如果没有 用[bits 32]() 来把代码编译成32位 的形式，当cpu 看到  mov bx, 0x10( BB 10 00 ) 的时候，会认为这是在操作32位的寄存器，
会继续向后找全 32位操作数，因此后续指令也都会出问题。

这也就应该是我代码中跳到保护模式 总是出错的原因：漏写了 bits 32