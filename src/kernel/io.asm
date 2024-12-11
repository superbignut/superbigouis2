[bits 32]
section .text

global _input_byte

_input_byte:
    push ebp
    mov ebp, esp                ; 保存 栈帧

    xor eax, eax                ; 清空
    mov edx, [ebp +8] 
    
    in al, dx                   ; 
    jmp $+2
    jmp $+2
    jmp $+2



    leave
    ret 