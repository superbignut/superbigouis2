+ 这一节的主要工作就是把原来在 loader 中写的 gdt 拷贝到新的位置，然后 在把新的位置加载到 gdtr 寄存器

问题大多出在前面，


```cpp
typedef struct segment_descriptor{      //  GDT表中的表项
    uint16_t limit_low;
    uint32_t base_low : 24;
    uint32_t segment_type : 4;          //  如果这32个字节不全都使用 uint32_t 会出问题
    uint32_t descriptor_type : 1;
    uint32_t DPL : 2;
    uint32_t segment_present : 1;
    uint8_t  limit_high : 4;
    uint8_t  AVL : 1;
    uint8_t  L : 1;
    uint8_t  D_B : 1;
    uint8_t  granularity : 1;
    uint8_t  base_high;
} _packed segment_descriptor;

```

最开始 只有 base_low 我给到了 uint32_t， 后面其余的我用的都是 uint8_t， 但是发现写出来的总的字节数不对 不是8个字节，并且问题就出在这个32位的类型不一致上，他们没有放在一起，后来把 uint32_t 一共32个位的都写成一个类型就ok了



+ 任务/进程 就是一个执行流，包括：
  + 程序入口地址
  + 堆栈
  + 寄存器信息


Calling Convention

> This is a short overview of the important calling convention details for the major System V ABI architectures. This is an incomplete account and you should consult the relevant processor supplement document for the details. Additionally, you can use the -S compiler option to stop the compilation process before the assembler is invoked, which lets you study how the compiler translates code to assembly following the relevant calling convention.

i386

> This is a 32-bit platform. The stack grows downwards. Parameters to functions are passed on the stack in reverse order such that the first parameter is the last value pushed to the stack, which will then be the lowest value on the stack. Parameters passed on the stack may be modified by the called function. Functions are called using the call instruction that pushes the address of the next instruction to the stack and jumps to the operand. Functions return to the caller using the ret instruction that pops a value from the stack and jump to it. The stack is 4-byte aligned all the time, on older systems and those honouring the SYSV psABI. On some newer systems, the stack is additionally 16-byte aligned just before the call instruction is called (usually those that want to support SSE instructions); consult your manual (GNU/Linux on i386 has recently become such a system, but code mixing with 4-byte stack alignment-assuming code is possible).

> Functions preserve the registers ebx, esi, edi, ebp, and esp; while eax, ecx, edx are scratch registers. The return value is stored in the eax register, or if it is a 64-bit value, then the higher 32-bits go in edx. Functions push ebp such that the caller-return-eip is 4 bytes above it, and set ebp to the address of the saved ebp. This allows iterating through the existing stack frames. This can be eliminated by specifying the -fomit-frame-pointer GCC option.

这里有指出 函数执行时需要保存 ebx, esi, edi, ebp, and esp 这5个寄存器， 并用 eax, ecx, edx 当作数据暂存寄存器

> As a special exception, GCC assumes the stack is not properly aligned and realigns it when entering main or if the attribute ((force_align_arg_pointer)) is set on the function.

> The direction flag must be clear before calling a function and is expected clear on exit too. This allows more efficient forward byte string operations. [1] [2] The System V ABI has the same requirement for 64-bit.




发现了奇怪的地方，只有第一次进入thread_a和thread_b的时候是因为task_create中设定的eip被ret切了进来，其余的AB打印都是因为保存的上下文回到While(true)循环；而且最开始设定的eip在进入thead_*的时候就被覆盖了，所以只能用一次。

但是由于thread本身不会再进入其他的函数，所以自己的保存的栈不会被破坏