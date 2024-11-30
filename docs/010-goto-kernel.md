用精炼一点的话总结的话就是分两步走：

    1. 写了一段32位的代码，然后编译成elf32的.o，并在这段代码中调用c的一个函数
    2. 写一段c的代码也编译成.o，包含汇编中调用的那个函数的函数体
    3. 最终把汇编和c语言链接到一起，成为bin文件，写到硬盘中就算ok了（当然这段代码要在loader中加载到内存中）

因此我们发现，比较关键的地方就是，怎么在汇编中调用c的函数， 并成功把两个重定位文件链接到一起，因此在up的视频中也可以看到，大部分时间都是在写新的makefile

---

1. asm 编译到 .o

        nasm -f elf32 $< -o $@

    其中 -f elf32 是编译到 32位的意思, 编译后 可以用 nm 命令 查看.o文件的符号表：

                λ nm -a ..\superbigouis2\build\kernel\start.o
                00000000 t .text
                00000000 T _start
                00000000 a src/kernel/start.asm

    这里一个简单的理解就是 "T" 符号意味着 数据在当前的文件中


    还可以使用 objdemp / readelf 查看.o文件的更多信息, 内容有点多， 先略过


2. start.o  和 main.o 链接成 kernel.bin

        ld.gold -m elf_i386 -static $^ -o $@ -Ttext $(ENTRY_POINT)

    这里我是发现的的windows中ld 没有 elf_i386的选项所以才用的同样是MinGW的ld_gold, 