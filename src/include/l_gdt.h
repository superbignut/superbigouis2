#ifndef LGDT_H_
#define LGDT_H_

#include "l_types.h"

#define GDT_SIZE 127                    //  最多可以是 2**13  取决于 segment selector的大小

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



typedef struct segment_selector{        //  选择GPD表中的哪一个GDT
    uint16_t RPL : 2;
    uint16_t TI : 1;
    uint16_t index : 13;
} _packed segment_selector;


typedef struct gdtr_content{            //  找到GDT表，lgdt指令加载到gdtr寄存器中
    uint16_t limit;
    uint32_t base_addr;
} _packed gdtr_content;

void gdt_init();

#endif