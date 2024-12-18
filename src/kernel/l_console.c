#include "l_console.h"


struct _console
{
    uint32_t screen_pos;        // 0xB8000 开始的绝对坐标 字节为单位
    uint32_t cursor_pos;        // 0xB8000 开始的绝对坐标 字节为单位
    
    uint8_t cursor_x;           // 0 开始 2字节为单位 相对坐标
    uint8_t cursor_y;           // 0 开始 2字节为单位 相对坐标

    // uint8_t attr;
    // uint16_t space;
};

struct _console console;


/// @brief 找到屏幕开始的地方， 真实地址保存到console
static void get_screen(){

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    console.screen_pos = read_byte_from_vga(CRT_DATA_REG_PORT) << 8;

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    console.screen_pos |= read_byte_from_vga(CRT_DATA_REG_PORT);
    
    console.screen_pos << 1;             //  * 2 一个字符两个字节

    console.screen_pos += MEM_ADDR_BASE; //  屏幕显示的字符在内存中真正开始的位置
}

/// @brief 设置屏幕位置为console 中的地址
/// @param _pos >= 0xb8000 的值 以 字节为单位
static void update_screen(){
    //console.screen_pos = _pos;
    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}


/// @brief 找到光标位置, 更新 cursor_x cursor_y cursor_pos
///        这里需要注意的地方是，因为屏幕起始的位置是可以调整的，但是光标始终是以0xb8000 为起点
///        因此需要减一下
static void get_cursor(){
    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX);
    console.cursor_pos = read_byte_from_vga(CRT_DATA_REG_PORT) << 8;

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX);
    console.cursor_pos |= read_byte_from_vga(CRT_DATA_REG_PORT);

    get_screen();

    console.cursor_pos << 1;
    console.cursor_pos += MEM_ADDR_BASE;

    uint32_t cursor_delta = (console.cursor_pos - console.screen_pos) >> 1;

    console.cursor_x = (uint8_t)(cursor_delta / SCREEN_WIDTH);
    console.cursor_y = (uint8_t)(cursor_delta % SCREEN_WIDTH);

}

/// @brief 设置光标位置
/// @param _pos >= 0xb8000 的值 以 字节为单位
static void update_cursor(){
    // console.cursor_pos = _pos;
    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX);
    write_byte_to_vga(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}



/// @brief 屏幕和光标都设置到起点 0xb8000
void console_clear(){
    console.screen_pos = MEM_ADDR_BASE;
    console.cursor_pos = MEM_ADDR_BASE;
    update_screen();
    update_cursor();

    uint16_t *ptr = (uint16_t *)MEM_ADDR_BASE;  //  每次指向2个字节

    while (ptr < MEM_ADDR_END)
    {
        *ptr++ = CONSOLE_SPACE;                 // 赋值空格
    }
}


/// @brief 
static void console_bs(){
    if(console.cursor_x > 0){

        console.cursor_pos -= 2;
        console.cursor_x -= 1;

        uint16_t *ptr = (uint16_t *)console.cursor_pos;
        *ptr = CONSOLE_SPACE;
        update_cursor();
    }
    
}


/// @brief 向下一行
static void console_lf(){
    if(console.cursor_y < SCREEN_HEIGHT - 1){

        console.cursor_pos += SCREEN_WIDTH_BYTE_SIZE;
        console.cursor_y += 1;

        update_cursor();
        return;
    }
}

/// @brief 回到行首
static void console_cr(){
    // uint32_t temp1 = MEM_ADDR_BASE + console.cursor_y * SCREEN_WIDTH_BYTE_SIZE;
    console.cursor_pos = console.cursor_pos - (console.cursor_x << 1);
    console.cursor_x = 0;
    update_cursor();
}



/// @brief 像屏幕写字符
/// @param buf 
/// @param count 
void console_write(char *buf, uint32_t count){
    char ch;
    char *ptr = (char *)(console.cursor_pos);
    while(count--){
        ch = *buf++;

        switch (ch)
        {
        case ASCII_NUL:
            break;
        case ASCII_BS:
            console_bs();
            break;
        default:
            if(console.cursor_x >= SCREEN_WIDTH){
                //console.cursor_x -= SCREEN_WIDTH;
                console_cr();
                console_lf();
            }
            
            *ptr++ = ch;
            *ptr++ = CONSOLE_DEFAULT_TEXT_ATTR;
            console.cursor_pos += 2;
            console.cursor_x += 1;
            break;
        }

        
    }
    update_cursor();
}

/// @brief 暂时只有屏幕清空
void console_init(){

    char a[] = "1";
    console_clear();    
    for(int i =0; i<=80; ++i){
        console_write(a, 1);
    }
    
    
    
}