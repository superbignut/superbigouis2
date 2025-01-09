[bits 32]
section .text

global _read_byte_from_port
global _read_word_from_port
global _write_byte_to_port
global _write_word_to_port

; 读到的数据 根据 ABI 约定， eax 用于存放函数的返回值
_read_byte_from_port:
    push ebp
    mov ebp, esp                ; esp 的值放到ebp 中
                                ; c 的话在这里会 subl 申请栈， 用于存放局部变量
    xor eax, eax                ; 清空
    mov edx, [ebp +8]           ; 四个字节的eip 和四个字节的 ebp， 把参数读到edx 
    
    in al, dx                   ; 读8位数据
    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret
_read_word_from_port:
    push ebp
    mov ebp, esp                ; esp 的值放到ebp 中
                                ; c 的话在这里会 subl 申请栈， 用于存放局部变量
    xor eax, eax                ; 清空
    mov edx, [ebp +8]           ; 四个字节的eip 和四个字节的 ebp， 把参数读到edx 
    
    in ax, dx                   ; 读16位数据
    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret 

_write_byte_to_port:
    push ebp
    mov ebp, esp                ; esp 的值放到ebp 中
                                ; c 的话在这里会 subl 申请栈， 用于存放局部变量
    mov edx, [ebp + 8]          ; port 16位
    mov eax, [ebp + 12]         ; value 8位
    
    out dx, al                  ; 把 al 写进端口
    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret 

_write_word_to_port:
    push ebp
    mov ebp, esp                ; esp 的值放到ebp 中
                                ; c 的话在这里会 subl 申请栈， 用于存放局部变量
    mov edx, [ebp + 8]          ; port 16位
    mov eax, [ebp + 12]         ; value 8位
    
    out dx, ax                  ; 把 ax 写进端口
    jmp $+2
    jmp $+2
    jmp $+2

    leave
    ret 