; 我这里的windows没有调试成功， 在start.asm中做的

[bits 32]

extern exit
global main

main:
    push 0
    call exit

    