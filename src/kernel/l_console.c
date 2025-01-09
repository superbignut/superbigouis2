#include "l_console.h"
#include "l_string.h"

struct _console
{
    uint32_t screen_pos;        // 0xB8000 开始的绝对坐标 字节为单位
    union
    {
        uint32_t cursor_pos;        // 0xB8000 开始的绝对坐标 字节为单位
        char *cursor_ptr;
    };
    
    uint8_t cursor_x;           // 0 开始 2字节为单位 相对坐标
    uint8_t cursor_y;           // 0 开始 2字节为单位 相对坐标

    // uint8_t attr;
    // uint16_t space;
};

struct _console console;


/// @brief 找到屏幕开始的地方， 真实地址保存到console
static void get_screen(){

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    console.screen_pos = read_byte_from_port(CRT_DATA_REG_PORT) << 8;

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    console.screen_pos |= read_byte_from_port(CRT_DATA_REG_PORT);
    
    console.screen_pos << 1;             //  * 2 一个字符两个字节

    console.screen_pos += MEM_ADDR_BASE; //  屏幕显示的字符在内存中真正开始的位置
}

/// @brief 设置屏幕位置为console 中的地址
/// @param _pos >= 0xb8000 的值 以 字节为单位
static void update_screen(){
    //console.screen_pos = _pos;
    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_HIGH_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_START_ADDR_LOW_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.screen_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}


/// @brief 找到光标位置, 更新 cursor_x cursor_y cursor_pos
///        这里需要注意的地方是，因为屏幕起始的位置是可以调整的，但是光标始终是以0xb8000 为起点
///        因此需要减一下
static void get_cursor(){
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

/// @brief 设置光标位置
/// @param _pos >= 0xb8000 的值 以 字节为单位
static void update_cursor(){
    // console.cursor_pos = _pos;
    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> (8 + 1)) & 0xff);

    write_byte_to_port(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX);
    write_byte_to_port(CRT_DATA_REG_PORT, ((console.cursor_pos - MEM_ADDR_BASE) >> 1) & 0xff);
}



/// @brief 屏幕和光标都设置到起点 0xb8000
void console_clear(){
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

/// @brief 在屏幕最后一行换行时触发, 滚动前清空
static void scroll_up(){

    //  屏幕外的一整行 没有超限
    if(console.screen_pos + SCREEN_SIZE_BYTE_SIZE + SCREEN_WIDTH_BYTE_SIZE < MEM_ADDR_END){
        
        //  先清空
        uint16_t *ptr = (uint16_t *)(console.screen_pos + SCREEN_SIZE_BYTE_SIZE);       //  取下一个屏幕的开始指针
        for(size_t i = 0; i < SCREEN_CHAR_WIDTH; ++i){
            *ptr++ = CONSOLE_SPACE;                                                     //  下一个行清空
        }

        console.screen_pos += SCREEN_WIDTH_BYTE_SIZE;
        console.cursor_pos += SCREEN_WIDTH_BYTE_SIZE;
    }
    else{
        //  超限，切到 MEM_BASE
        memory_copy((void *)MEM_ADDR_BASE, (void *)console.screen_pos, SCREEN_SIZE_BYTE_SIZE);  //  当前屏幕拷贝到初始屏幕
        console.cursor_pos -= (console.screen_pos - MEM_ADDR_BASE);             //  这里其实没换行
        console.screen_pos = MEM_ADDR_BASE;                                     //  只进行切换

    }
    update_screen();
    update_cursor();    
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
    if(console.cursor_y < SCREEN_CHAR_HEIGHT - 1){

        console.cursor_pos += SCREEN_WIDTH_BYTE_SIZE;
        console.cursor_y += 1;

        update_cursor();
        return;
    }
    scroll_up();
}

/// @brief 回到行首
static void console_cr(){
    // uint32_t temp1 = MEM_ADDR_BASE + console.cursor_y * SCREEN_WIDTH_BYTE_SIZE;
    console.cursor_pos = console.cursor_pos - (console.cursor_x << 1);
    console.cursor_x = 0;
    update_cursor();
}

/// @brief 
static void console_del(){
    // uint32_t temp1 = MEM_ADDR_BASE + console.cursor_y * SCREEN_WIDTH_BYTE_SIZE;
    // Todo
}



/// @brief 像屏幕写字符
/// @param buf 
/// @param count 
void console_write(char *buf, uint32_t count){
    char ch;
    // char *ptr = (char *)(console.cursor_pos);                        //  debug
    while(count--){
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
            if(console.cursor_x >= SCREEN_CHAR_WIDTH){
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
}

/// @brief 暂时只有屏幕清空
void console_init(){

    char _clear[] = "console init...\n";
    console_clear();    
    /* while (True)
    {
        console_write(c, 2);
    } */
    console_write(_clear, string_len(_clear));

}