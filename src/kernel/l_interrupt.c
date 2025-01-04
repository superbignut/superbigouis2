#include "l_interrupt.h"
#include "l_debug.h"

gate_descriptor idt[IDT_SIZE];              //  中断描述符表

gdtr_content idt_ptr;                       //  idt 选择子

extern void interrupt_handler();            //  中断向量


/// @brief 这里主要把idt 表中的前 256个中断全部都指向了同一个 中断处理函数
void interrupt_init(){

    for(size_t i = 0; i< IDT_SIZE; ++i){
        
        gate_descriptor *gate = &idt[i];
        gate->offest_low = (uint32_t)interrupt_handler & 0xffff;
        gate->offest_high = ((uint32_t)interrupt_handler >> 16) & 0xffff;
        gate->segment_selector = 1 << 3;    //  1 2 都可以，见 loader                            
        gate->reserved = 0;
        gate->type = 0b1110;                //  32bit中断门， 这里如果是 16位的代码，最高位置0 感觉也能切进去
        gate->segment = 0;
        gate->DPL = 0;
        gate->P =  1;
    }
    idt_ptr.base_addr = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;        //  极限是 8N-1 手册中有说明
    
    // XBB;
    asm volatile("lidt _idt_ptr");          //  加载 idtr 寄存器
}