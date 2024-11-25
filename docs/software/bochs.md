[window-bochs-debug](https://blog.csdn.net/baidu_19473529/article/details/124466879)

+ 在ubuntu上是修改 options或者重新编译来进入bochs的debug模式的，但是windows的这个版本，是在文件夹中使用 bochsdbg.exe来调试


[命令行调试](https://bochs.sourceforge.io/doc/docbook/user/internal-debugger.html)


+ r sreg s c xp b u

        xp/10 0x7c00 #  查看内存0x7c00的位置
        u/10 0x7c00 # 查看指令
        b # 打断点
        s # 单步
        c # continue
        sreg # 段寄存器
        r # 通用寄存器4
        print-stack # 查看栈

> x  /nuf addr      Examine memory at linear address addr>
> xp /nuf addr      Examine memory at physical address addr