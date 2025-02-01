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


+ 补充查看 gdt 和 page 的 命令：


  r|reg|regs|registers         List of CPU integer registers and their contents
  fp|fpu                       List of all FPU registers and their contents
  mmx                          List of all MMX registers and their contents
  sse|xmm                      List of all SSE registers and their contents
  ymm|zmm                      List of all AVX registers and their contents
  amx|tile n                   Show AMX state and TILE register contents
  sreg                         Show segment registers and their contents
  dreg                         Show debug registers and their contents
  creg                         Show control registers and their contents

  info cpu                     List of all CPU registers and their contents
  info eflags                  Show decoded EFLAGS register
  info break                   Information about current breakpoint status
  info tab                     Show paging address translation
  info idt                     Show contents of the IDT
  info gdt                     Show contents of the GDT
  info ldt                     Show contents of the LDT
  info device                  Show state of the specified device