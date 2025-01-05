#include "l_interrupt.h"
#include "l_debug.h"
#include "l_printk.h"

gate_descriptor idt[IDT_SIZE];              //  中断描述符表

gdtr_content idt_ptr;                       //  idt 选择子

// extern void interrupt_handler();                             //  中断向量

extern handler_t handler_entry_table[HANDLER_ENTRY_SIZE];       //  中断处理函数入口地址 idt.handler -> handler_entry_table[i] -> handler_table[i] -> exception_handler()
                                                                //  汇编中实现的主要功能是，判断是否压入 错误码，跳转回来


handler_t handler_table[IDT_SIZE];                              //  真正的处理函数位置

/// @brief 
static void exception_handler(){
    printk("yes...");
    while(True);
}

/// @brief 对前32个异常初始化，handler函数 为汇编中定义的 _interrupt_handler_0x**
void interrupt_init(){

    for(size_t i = 0; i< HANDLER_ENTRY_SIZE; ++i){
        
        gate_descriptor *gate = &idt[i];
        handler_t handler = handler_entry_table[i];

        gate->offest_low = (uint32_t)handler & 0xffff;
        gate->offest_high = ((uint32_t)handler >> 16) & 0xffff;
        gate->segment_selector = 1 << 3;    //  1 2 都可以，见 loader                            
        gate->reserved = 0;
        gate->type = 0b1110;                //  32bit中断门， 这里如果是 16位的代码，最高位置0 感觉也能切进去
        gate->segment = 0;
        gate->DPL = 0;
        gate->P =  1;
    }
    
    for(size_t i = 0; i< HANDLER_ENTRY_SIZE; ++i){
        handler_table[i] = exception_handler;
    }

    idt_ptr.base_addr = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;        //  极限是 8N-1 手册中有说明
    
    // XBB;
    asm volatile("lidt _idt_ptr");          //  加载 idtr 寄存器
}