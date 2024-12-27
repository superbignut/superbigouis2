#include "l_gdt.h"
#include "l_assert.h"
#include "l_debug.h"
#include "l_string.h"
#include "l_printk.h"

segment_descriptor gdt[GDT_SIZE];                           //  GDT 表

gdtr_content gdt_ptr;                                       //  GDT 表首地址



/// @brief 将汇编 loader 中的 gdt 表 copy 到 c 中新的位置，并使用 lgdt 重新加载
void gdt_init(){
    
    assert(sizeof(segment_descriptor) == 8);                //  gdt_des 的大小是8个字节
    assert(sizeof(segment_selector) == 2);                  //  segment_sel 的大小是2个字节
    assert(sizeof(gdtr_content) == 6);                      //  gdtr_content 的大小是2个字节

    asm volatile("sgdt _gdt_ptr");                          //  windows 增加前缀；把gdtr保存到 _gdt_ptr指针处

    /*
        As with segments, the limit value is added to the base address to get the address of the last valid byte. 
        A limit value of 0 results in exactly one valid byte. Because segment descriptors are always 8 bytes long, 
        the GDT limit should always be one less than an integral multiple of eight (that is, 8N – 1).
    */

    memory_copy((void *)gdt, (void *)gdt_ptr.base_addr, gdt_ptr.limit + 1);             //  加载老的gdt表到 新的gdt表中

    gdt_ptr.base_addr = (uint32_t)gdt;

    gdt_ptr.limit = sizeof(gdt) - 1;

    asm volatile("lgdt _gdt_ptr");
    
    printk("gdt init ...\n");
} 