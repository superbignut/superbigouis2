    global _task_switch, _task_switch_deprecated
      
_task_switch:                   ;  传入的参数是 next 表示需要切换的函数的页的首地址
_task_switch_deprecated:
        push ebp
        mov ebp, esp           ;  保存栈帧


        push ebx 
        push esi
        push edi

        mov eax, esp
        and eax, 0xffff_f000    ;  找到当前的页
        mov [eax], esp          ;  页内前四个字节是栈顶地址, 保存上下文 说的就是这里，将当前的栈顶保存到第一个字节

        mov eax, [ebp + 8]      ;  ebp+0 存的是最开始push 的ebp; ebp+4存的是eip ebp+8存的是传入的参数， 所以是获取参数
        mov esp, [eax]          ;  把 next页内前四个字节， 也就是next的栈顶的上下文 task_frame_t 恢复， 这里其实不仅改了栈顶，进而间接影响了 之前压入栈的 所有寄存器
                                ;  副作用就会体现在，最后的 pop 和 ret 上


        pop edi
        pop esi
        pop ebx
        pop ebp
                                ;  接下来的栈顶是 需要去执行的 （ebp+4):eip 和 (ebp+8):传递的参数
        ret                     ;  所以这里的ret 其实是将 next 中的 frame->eip = (void *)target; 切了进来 进而跳转到新的任务开始执行
                                ;  完成 task_switch 功能， 这里进入到新的函数后，栈的地址一定会再次减小，所以

