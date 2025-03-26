/**
 * @file l_console.c
 * @author bignut
 * @brief This is c function file which control the movement/behaviour of screen/console and cursor by r/w vga-registers.
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */ 
#include "l_console.h"
#include "l_string.h"
#include "l_interrupt.h"

/**
 * @brief  Console-specific struct used to s/l screen and cursor positions.
 */
struct _console
{
    uint32_t screen_pos;    /**< 0xB8000 开始的绝对坐标 字节为单位， 用来标志屏幕的起点 */    
    /**
        Union type used to represent cursor_pose, union is used to convert u32_t to 32bit_pointer.
    */
    union
    {
        uint32_t cursor_pos;    /**< 0xB8000 开始的绝对坐标 字节为单位 */
        char *cursor_ptr;       /**< 0xB8000 开始的绝对坐标 字节为单位 但是当作指针读取 */
    };                          

    uint8_t cursor_x;   /**< 0 开始 2字节为单位 相对坐标*/ 

    uint8_t cursor_y;   /**< 0 开始 2字节为单位 相对坐标*/ 
};


struct _console console;   /**< Global console variable to sl screen and cursor infos.*/



/**
 * @brief 找到屏幕开始的地方， 真实地址保存到全局的 console
 * 
 * @details 这里的 sl 操作的目的是为了有的时候屏幕想要 实现滚屏的效果 结合 update_screen 使用
 * 
 */
static void get_screen()
{

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    console.screen_pos = read_byte_from_port(CRT_DATA_REG_PORT) << 8;

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    console.screen_pos |= read_byte_from_port(CRT_DATA_REG_PORT);
    
    console.screen_pos << 1;    // 相当于 * 2 因为一个字符占两个字节

    console.screen_pos += MEM_ADDR_BASE;    // 屏幕显示的字符在内存中真正开始的位置
}

/**
 * @brief 设置屏幕位置为 console 中的地址
 * 
 * @details 由于设计的原意， 因此每次需要先在本函数之前 sl console 全局变量， 进而 调用此函数
 * 
 * @param _pos 需要 >= 0xb8000 的值 以字节为单位 
 * 
 */
static void update_screen()
{
    //console.screen_pos = _pos;
    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);    // 这里的右移 + 1 同样为字节和字符的原因

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}

/**
 * @brief 访问寄存器，得到光标位置，结合 update_cursor 一起使用
 * 
 * @details 这里需要注意的地方是，因为屏幕起始的位置是可以调整的，但是光标始终是以0xb8000 为起点 因此需要减一下
 * 
 */
static void get_cursor()
{
    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX);
    console.cursor_pos = read_byte_from_port(CRT_DATA_REG_PORT) << 8;

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX);
    console.cursor_pos |= read_byte_from_port(CRT_DATA_REG_PORT);

    get_screen();

    console.cursor_pos << 1;
    console.cursor_pos += MEM_ADDR_BASE;

    uint32_t cursor_delta = (console.cursor_pos - console.screen_pos) >> 1;

    console.cursor_x = (uint8_t)(cursor_delta / SCREEN_CHAR_WIDTH);
    console.cursor_y = (uint8_t)(cursor_delta % SCREEN_CHAR_WIDTH);
}

/**
 * @brief 设置光标位置, 先修改 全局 console 再调用函数
 * 
 * @param _pos 需满足 >= 0xb8000 的值 以字节为单位
 */
static void update_cursor()
{
    // console.cursor_pos = _pos;
    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}

/**
 * @brief 屏幕和光标都设置到起点 0xb8000， 并对所有字符作清空操作
 * 
 */
void console_clear()
{
    console.screen_pos = MEM_ADDR_BASE;
    console.cursor_pos = MEM_ADDR_BASE;
    update_screen();
    update_cursor();

    uint16_t *ptr = (uint16_t *)MEM_ADDR_BASE;  //  每次指向2个字节

    while (ptr < (uint16_t *)MEM_ADDR_END)
    {
        *ptr++ = CONSOLE_SPACE;                 // 赋值空格
    }
}


/**
 * @brief 在屏幕滚动到最后一行换行时触发, 滚动前清空
 */
static void scroll_up()
{

    //  屏幕外的一整行 没有超限
    if(console.screen_pos + SCREEN_SIZE_BYTE_SIZE + SCREEN_WIDTH_BYTE_SIZE < MEM_ADDR_END)
    {
        
        //  先清空
        uint16_t *ptr = (uint16_t *)(console.screen_pos + SCREEN_SIZE_BYTE_SIZE);       //  取下一个屏幕的开始指针
        for(size_t i = 0; i < SCREEN_CHAR_WIDTH; ++i)
        {
            *ptr++ = CONSOLE_SPACE;                                                     //  下一个行清空
        }

        console.screen_pos += SCREEN_WIDTH_BYTE_SIZE;
        console.cursor_pos += SCREEN_WIDTH_BYTE_SIZE;
    }
    else
    {
        //  超限，切到 MEM_BASE
        memory_copy((void *)MEM_ADDR_BASE, (void *)console.screen_pos, SCREEN_SIZE_BYTE_SIZE);  //  当前屏幕拷贝到初始屏幕
        console.cursor_pos -= (console.screen_pos - MEM_ADDR_BASE);             //  这里其实没换行
        console.screen_pos = MEM_ADDR_BASE;                                     //  只进行切换

    }
    update_screen();
    update_cursor();    
}

/**
 * @brief 退格/删除
 * 
 */
static void console_bs()
{
    if(console.cursor_x > 0)
    {

        console.cursor_pos -= 2;
        console.cursor_x -= 1;

        uint16_t *ptr = (uint16_t *)console.cursor_pos;
        *ptr = CONSOLE_SPACE;
        update_cursor();
    }
    
}


/**
 * @brief 向下一移位， 与 cr 结合后是完整的回车
 * 
 */
static void console_lf()
{
    if(console.cursor_y < SCREEN_CHAR_HEIGHT - 1)
    {

        console.cursor_pos += SCREEN_WIDTH_BYTE_SIZE;
        console.cursor_y += 1;

        update_cursor();
        return;
    }
    scroll_up();
}

/// @brief 回到行首
/**
 * @brief 
 * 
 */
static void console_cr()
{
    // uint32_t temp1 = MEM_ADDR_BASE + console.cursor_y * SCREEN_WIDTH_BYTE_SIZE;
    console.cursor_pos = console.cursor_pos - (console.cursor_x << 1);
    console.cursor_x = 0;
    update_cursor();
}


/**
 * @brief Todo
 * 
 */
static void console_del()
{
    // uint32_t temp1 = MEM_ADDR_BASE + console.cursor_y * SCREEN_WIDTH_BYTE_SIZE;
    // Todo
}



/**
 * @brief 屏幕写字符
 * 
 * @param buf 待打印字符的指针
 * 
 * @param count 待打印字符长度
 */
void console_write(char *buf, uint32_t count)
{
    bool tmp = interrupt_disable();                                     //  获取当下的中断状态， 并在最后进行恢复
    char ch;
    // char *ptr = (char *)(console.cursor_pos);                        //  debug
    while(count--)
    {
        ch = *buf++;

        switch (ch)
        {
        case ASCII_NUL:
            break;
        case ASCII_BS:
            console_bs();
            break;
        case ASCII_DEL:
            console_del();
            break;
        // case ASCII_CR:       // 这里其实回车和换行应该是都可以的
        case ASCII_LF:
            console_lf();
            console_cr();
            break;
        default:
            if(console.cursor_x >= SCREEN_CHAR_WIDTH)
            {
                //console.cursor_x -= SCREEN_WIDTH;
                console_cr();
                console_lf();
            }
            
            *console.cursor_ptr++ = ch;                                 //  修改指针的同时修改 cursor_pos
            *console.cursor_ptr++ = CONSOLE_DEFAULT_TEXT_ATTR;

            // console.cursor_pos += 2;
            console.cursor_x += 1;
            break;
        }
    }
    update_cursor();

    set_if_flag(tmp);                                                   //  恢复之前的中断状态
}


/**
 * @brief 屏幕清空， 打印屏幕初始化字符
 * 
 */
void console_init()
{

    char _clear[] = "\n#### CONSOLE INIT...\n";
    console_clear();    
    /* while (True)
    {
        console_write(c, 2);
    } */
    console_write(_clear, string_len(_clear));

}