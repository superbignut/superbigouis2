#include "l_interrupt.h"
#include "l_debug.h"
#include "l_printk.h"
#include "l_assert.h"
#include "l_task.h"

gate_descriptor idt[IDT_SIZE];              //  中断描述符表

gdtr_content idt_ptr;                       //  idt 选择子

// extern void interrupt_handler();                             //  中断向量

extern handler_t handler_entry_table[HANDLER_ENTRY_SIZE];       //  中断处理函数入口地址 idt.handler -> handler_entry_table[i] -> handler_table[i] -> exception_handler()
                                                                //  汇编中实现的主要功能是，判断是否压入 错误码，跳转回来


handler_t handler_table[IDT_SIZE];                              //  真正的处理函数位置

/// @brief 用于显示异常信息
static char *msg[]=
{
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
/*

    push 0x2222_2222                ; 原本不压入错误码的，也压入一个
        
    push %1                         ; 异常的编号，也当作了传给c的函数的参数
        
    %endmacro
    push ds
    push es 
    push fs 
    push gs

    pusha                                   ;  EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    
    输入参数 反着来
*/
static void exception_handler(int vector, uint32_t edi, uint32_t dsi, uint32_t ebp, uint32_t esp,
                              uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax,
                              uint32_t gs, uint32_t fs, uint32_t es, uint32_t ds,
                              uint32_t vector_same, uint32_t code)
{
    if(vector < 22)
    {
        printk("Exception occured: %s, Error Code: 0x%x\n", msg[vector], code);
    }
    else
    {
        panic("Exception occured: %s\n", "Error code not defined.");
    }
    while(True);
}

/// @brief 通知中断控制器，中断处理结束，重置 ISR 位
/// @param vector 
void send_eoi(int vector)
{
    if(vector >= 0x20 && vector < 0x28)
    {
        write_byte_to_port(PIC_8259_MASTER_COMMAND, PIC_8259_EOI);
    }
    if(vector >= 0x28 && vector < 0x30)
    {
        write_byte_to_port(PIC_8259_MASTER_COMMAND, PIC_8259_EOI);
        write_byte_to_port(PIC_8259_SLAVE_COMMAND, PIC_8259_EOI);
    }
}


// uint32_t _cnt = 0;
// extern void schedule();

/// @brief 外部中断 处理函数
/// @param vector 
static void hardware_int_handler(int vector)
{
    
    send_eoi(vector);
    // schedule();
    // printk("hardware_int_handler was called %d times.\n", _cnt++);
}

/// @brief 对前32个异常初始化，handler函数 为汇编中定义的 _interrupt_handler_0x**
static void idt_init()
{

    for(size_t i = 0; i< HANDLER_ENTRY_SIZE; ++i)
    {
        
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
    for(size_t i = 0; i < EXCEPTION_SIZE; ++i)
    {
        handler_table[i] = exception_handler;
    }

    //  初始化外部中断处理函数
    for(size_t i = EXCEPTION_SIZE; i < HANDLER_ENTRY_SIZE; ++i)
    {
        handler_table[i] = hardware_int_handler;
    }

    idt_ptr.base_addr = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;        //  极限是 8N-1 手册中有说明
    
    // XBB;
    asm volatile("lidt _idt_ptr");          //  加载 idtr 寄存器
}



/// @brief 初始化中断控制器， 初始化 8259A 的 ICW 和 OCW 最主要的工作就是 设定了外部中断的中断号，进而和异常就统一在一起了
static void pic_init()
{
    write_byte_to_port(PIC_8259_MASTER_COMMAND, 0b00010001);    //  ICW1: 边沿触发 + 级联 + 需要ICW4
    write_byte_to_port(PIC_8259_MASTER_DATA, 0x20);             //  ICW2: 0b0010_0000 因此后续的编号都是 0x20 + IRX 编号 **核心**
    write_byte_to_port(PIC_8259_MASTER_DATA, 0b00000100);       //  ICW3: IR2 接子片
    write_byte_to_port(PIC_8259_MASTER_DATA, 0b00000001);       //  ICW4: 8086 + 手动 EOI

    write_byte_to_port(PIC_8259_SLAVE_COMMAND, 0b00010001);     //  ICW1: 边沿触发
    write_byte_to_port(PIC_8259_SLAVE_DATA, 0x28);              //  ICW2：中断编号 **核心**
    write_byte_to_port(PIC_8259_SLAVE_DATA, 2);                 //  ICW3: 连接到主片 IR2
    write_byte_to_port(PIC_8259_SLAVE_DATA, 0b00000001);        //  ICW4: 8086 + 手动 EOI

    write_byte_to_port(PIC_8259_MASTER_DATA, 0b11111111);       //  OCW1：MASK 只保留 0 号中断
    write_byte_to_port(PIC_8259_SLAVE_DATA, 0b11111111);        //  OCW2: MASK
}


/// @brief 设置外部中断处理函数，clock 中断从0开始
/// @param irq 
/// @param handler 
void set_hardware_interrupt_handler(uint32_t irq, handler_t handler)
{
    assert(irq >=0 && irq < 16);
    handler_table[EXCEPTION_SIZE + irq] = handler;  //  从 异常之后的中断号开始写
}

/// @brief 设置屏蔽 MASK，先读已有配置，再添加新的配置
/// @param irq 
/// @param if_enable 
void set_hardware_interrupt_mask(uint32_t irq, bool if_enable)
{
    assert(irq >=0 && irq < 16);
    uint32_t _port;
    uint8_t _data;

    if(irq < 8)
    {
        _port = PIC_8259_MASTER_DATA;
    }
    else
    {
        _port = PIC_8259_SLAVE_DATA;
        irq -= 8;                       //  这时需要去屏蔽 8259从片
    }

    _data = 1 << irq;

    if(if_enable)
    {
        write_byte_to_port(_port, read_byte_from_port(_port) & (~_data));   //  先读出来，再去写， 可以防止覆盖已有的设置
    }
    else
    {
        write_byte_to_port(_port, read_byte_from_port(_port) | (_data));
    }

}

/// @brief 关闭 if 位，返回原 if 位
/// @return 
bool interrupt_disable()
{
    asm volatile(
        "pushfl\n"        // eflag 入栈
        "cli\n"           // 关中断
        "popl %eax\n"     // pop 到 eax
        "shrl $9, %eax\n" // 右移 9 位
        "andl $1, %eax\n" // 与 1
    );
}

/// @brief 返回 if 位
/// @return 
bool get_if_flag()
{
        asm volatile(
        "pushfl\n"        // eflag 入栈
        "popl %eax\n"     // pop 到 eax
        "shrl $9, %eax\n" // 右移 9 位
        "andl $1, %eax\n" // 与 1
    );
}

/// @brief flag = true 开中断，= flase 关中断
/// @param flag 
/// @return 
bool set_if_flag(bool flag)
{
    if(flag)
    {
        asm volatile("sti\n");
    }
    else
    {
        asm volatile("cli\n");
    }
}

/// @brief 中断初始化
void interrupt_init()
{
    idt_init();
    pic_init();
    printk("#### IDT AND PIC INIT...\n");
}