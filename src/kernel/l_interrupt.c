#include "l_interrupt.h"
#include "l_debug.h"
#include "l_printk.h"
#include "l_assert.h"

gate_descriptor idt[IDT_SIZE];              //  中断描述符表

gdtr_content idt_ptr;                       //  idt 选择子

// extern void interrupt_handler();                             //  中断向量

extern handler_t handler_entry_table[HANDLER_ENTRY_SIZE];       //  中断处理函数入口地址 idt.handler -> handler_entry_table[i] -> handler_table[i] -> exception_handler()
                                                                //  汇编中实现的主要功能是，判断是否压入 错误码，跳转回来


handler_t handler_table[IDT_SIZE];                              //  真正的处理函数位置

/// @brief 用于显示异常信息
static char *msg[]={
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception"
};



/// @brief 真实的异常处理函数的位置，根据异常编号打印异常名
static void exception_handler(int vector, int code){
    
    if(vector < 22){
        printk("Exception occured: %s, Error Code: 0x%x\n", msg[vector], code);
    }else{
        panic("Exception occured: %s\n", "Error code not defined.");
    }
    while(True);
}

/// @brief 外部中断 处理函数
/// @param vector 
static void hardware_int_handler(int vector){

}

/// @brief 对前32个异常初始化，handler函数 为汇编中定义的 _interrupt_handler_0x**
static void idt_init(){

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
    
    //  初始化异常中断处理函数
    for(size_t i = 0; i< HANDLER_ENTRY_SIZE; ++i){
        handler_table[i] = exception_handler;
    }

    //  初始化外部中断处理函数
    for(size_t i = 0x20; i < HANDLER_ENTRY_SIZE; ++i){
        handler_table[i] = hardware_int_handler;
    }

    idt_ptr.base_addr = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;        //  极限是 8N-1 手册中有说明
    
    // XBB;
    asm volatile("lidt _idt_ptr");          //  加载 idtr 寄存器
}

static void pic_init(){
    
}

void interrupt_init(){

    idt_init();

}