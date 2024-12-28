gdb指令




+ x/10xw 0xffa8 查看地址

+ x/10i $eip    查看指令

+ info register/stack 查看寄存器

+ -exec display/16xw $sp 这个display命令好像就是会每次debug执行的时候都会刷一次