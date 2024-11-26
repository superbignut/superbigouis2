#### 开发环境配置：
---
+ bochs，nasm，qemu，dd 都有windows的版本，不妨用windows试一试。

+ 之前跟着up写的os和x86汇编的时候，也记过笔记但是记在一起感觉很乱，这次重新实现，打算分好类，把笔记做的好一些，方便查看和修改

---
1. 编译 boot.asm  

        nasm -f bin boot.asm -o boot.bin 
        # 这里的目的是测试一下nasm，输出16位的为2进制指令到bin文件，使用hex-editor查看， nasm 的使用在nasm.md

2. 创建硬盘镜像: 大小16M，每个区块512字节，flat应该是最普通的模式

        bximage -q -hd=16 -func=create -secsize=512 -imgmode=flat master.img
        # bximage 是bochs 配套的镜像文件创建工具，创建后会生成一串配置文本，需要写到bochs的启动文件中，如下：
        # ata0-master: type=disk, path="master.img", mode=flat

3. boot.bin 写入主引导扇区， 也就是前512个字节，

        dd if=boot.bin of=master.img bs=512
        # 这里使用的是dd for windows 参数会和 linux 的 dd 命令不太一样， 但功能都有，这里是将bin文件写到img 文件中的前512个字节


[byte 关键字]()
                mov byte [ds:0], 'H'
        找不到官方的说明，但是不加会报错，因为无法确定到底要放到多大的空间里，如果指定了byte 则就只放在第一个字节里

[ds省略]()
        如果ds省略的话，也是up 的写法
                mov byte [0], 'H'
        编译出来， 使用 u/15 0x7c00 查看编译后的代码，发现还是有ds在里面，所以加上更清晰一点
                0000000000007c18: (                    ): mov byte ptr ds:0x0000, 0x48 ; c606000048

[$$ 关键字]()
                mov bx, $$
        测试了一下上面的，$$ 是0x7c00, 如果没标记段的话，应该就是最开头的地址，说不定默认就是 .text或者 .data段
<!--
    [org 0x7c00]

    mov ax, 3
    int 0x10

    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov ax, 0xb800
    mov ds, ax
    mov byte [0], 'H'

    jmp $

    times 510 - ($-$$) db 0

    db 0x55, 0xaa
 -->
[005-printf-in-real-mode]()

这里仍然是调用bios 的中断函数 0x10 ，具体细节补充到bios文档中


<!-- 
    [org 0x7c00]

    mov ax, 3
    int 0x10
    xchg bx, bx
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov si, hello_str
    call real_printf

    jmp $



real_printf:
    ; si用于存放字符串首地址， 字符串用0表示结束
    mov cx, 0
    mov ds, cx
    mov ah, 0x0e
    
.next:
    mov byte al, [ds:si]
    cmp al, 0  
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

print:
    mov ax, 0
    mov ds, ax
    mov ax, 0xb800
    mov ds, ax
    mov byte [ds:0], 'H'
    iret

hello_str:
    db 'H', 'E', 'L', 'L', 'O', 0

    times 510 - ($-$$) db 0

    db 0x55, 0xaa



 -->