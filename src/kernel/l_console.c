#include "l_console.h"


struct _console
{
    uint32_t screen_pos;
    uint32_t cursor_pos;
    
    uint8_t x;
    uint8_t y;

    uint8_t attr;
    uint16_t space;
};

struct _console console;


/// @brief 找到屏幕开始的地方， 使用内存中的真实地址进行保存
static void get_screen(){

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    console.screen_pos = read_byte_from_vga(CRT_DATA_REG_PORT) << 8;

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    console.screen_pos |= read_byte_from_vga(CRT_DATA_REG_PORT);
    
    console.screen_pos << 1;             //  * 2 一个字符两个字节

    console.screen_pos += MEM_ADDR_BASE; //  屏幕显示的字符在内存中真正开始的位置
}

/// @brief 设置屏幕开始的位置
static void set_screen(){

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}


void console_init(){
    console.screen_pos = 0;             // 屏幕开始显示的真实地址（内存中的真实地址）
    console.cursor_pos = 0;
    console.x = 0;
    console.y = 0;
    console.attr = 7;
    console.space = 0x0720;
    console_clear();
    
    console.screen_pos = MEM_ADDR_BASE +  SCREEN_WIDTH_BYTE_SIZE;
    set_screen();
}

void console_clear(){
    get_screen();
}

void console_write(char *buf, uint32_t count){



}
