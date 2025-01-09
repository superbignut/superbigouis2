#ifndef LINTERRUPT_H_
#define LINTERRUPT_H_
#include "l_types.h"
#include "l_gdt.h"              //  这里的 selector 都是一样的

#define IDT_SIZE 256
#define HANDLER_ENTRY_SIZE 0x30

typedef void *handler_t;

#define PIC_8259_MASTER_COMMAND 0x20
#define PIC_8259_MASTER_DATA 0x20

#define PIC_8259_SLAVE_COMMAND 0xA0
#define PIC_8259_SLAVE_DATA 0xA1

#define PIC_8259_EOI 0x20    

typedef struct gate_descriptor{
    uint16_t offest_low;                    //  偏移低地址
    uint16_t segment_selector;              //  这里对应的是 gdt 中放在 cs 等段寄存器中的 selector
    uint8_t  reserved;                      //  0
    uint8_t  type : 4;                      //  中断类型 0b1110 但最高位似乎是16和32的区别 Size of gate
    uint8_t  segment : 1;                   //  0
    uint8_t  DPL : 2;                       //  优先级
    uint8_t  P : 1;                         //  是否有效
    uint16_t offest_high;                   //  高位偏移
} _packed gate_descriptor;


void interrupt_init();


#endif
