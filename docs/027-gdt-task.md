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

+ 这段代码真的最开始看不懂，很多感觉多余的地方，最后发现都是巧妙之处


```asm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        push * 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 保存当前栈顶 代码1
        mov eax, esp
        and eax, 0xffff_f000    
        mov [eax], esp          
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 切换到参数的栈顶 代码2
        mov eax, [ebp + 8]      
        mov esp, [eax]          
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        pop * 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;        
        ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
```
        
                              
大致流程是 

最开始 -> 第一次 A -> 第一次 B -> 第二次 A -> ...


1. 最开始 -> 第一次 A

    这里保存上下文没什么用，但是切换栈顶会再最后 ret 的时候进入 thread_a 函数，去答应A，然后进入 schedule ，进而进入 task_switch


2. 第一次 A -> 第一次 B

    这里如果删掉中间的代码，只看 push *4 和 pop * 4 ，那么最后的 ret 会很自然的回到 原来 task_switch 的结束的地方，进而回到
    schedule 调用的地方，也就相当于这个switch 函数什么都没做
    
    但是， 保存上下文的作用(代码1)就在于, push * 4 之后，我在这里先把 栈顶指针记住，进而如果栈顶到栈底的所有的值都不变，那么当我下次把栈顶恢复的时候，再去pop * 4
    那么再去 ret ，就不会有什么不一样， 因为栈没变，所以这个ret的位置也没变，进而这个ret 后面的流程都不变，其实挺像 jmp出去 ，又jmp回来的感觉

3. 第一次 B -> 第二次 A
    由于 A 之前的保存的上下文 在页的前4个字节，那么再恢复的时候，就同样拿前4个字节，就ok了

        mov [eax], esp  
        mov esp, [eax]  

    这两段代码就是成对应的关系，存入上下文，和恢复上下文


``` cpp
uint32_t thread_a(){
    while(True){
        printk("A");
        schedule();
                                                        //  汇编中保存的上下文，1 执行完之后 回到 2 
        printk("C");
    }
}


/// @brief 同上
/// @return 
uint32_t thread_b(){
    while(True){
        printk("B");
        schedule();

        printk("D");
    }
}

如果将代码改成这样就可以返现

最开始是A

进而是B

进而是 CA

进而是DB

然后是 CA DB 开始循环

```

  
ABI 有待补充