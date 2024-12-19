#ifndef LCONSOLE_H_
#define LCONSOLE_H_
#include "l_types.h"
#include "l_io.h"

#define MEM_ADDR_BASE 0xB8000
#define MEM_ADDR_SIZE 0x8000                            // 32k
#define MEM_ADDR_END (MEM_ADDR_BASE + MEM_ADDR_SIZE)

#define SCREEN_CHAR_WIDTH 80             // 可显示字符宽度
#define SCREEN_CHAR_HEIGHT 25            // 可显示字符高度


#define SCREEN_WIDTH_BYTE_SIZE (SCREEN_CHAR_WIDTH * 2)       // 字节数
#define SCREEN_HEIGHT_BYTE_SIZE (SCREEN_CHAR_HEIGHT * 2)
#define SCREEN_SIZE_BYTE_SIZE (SCREEN_HEIGHT_BYTE_SIZE * SCREEN_WIDTH_BYTE_SIZE)

#define ASCII_NUL 0x00      // NULL
#define ASCII_ESC 0x1B      // ESC
#define ASCII_BS  0x08      // Backspace
#define ASCII_HT  0x09      // Horizontal Tab
#define ASCII_LF  0x0A      // Line Feed
#define ASCII_VT  0x0B      // Vertical Tab
#define ASCII_FF  0x0C      // Form Feed
#define ASCII_CR  0x0D      // Carriage Return 
#define ASCII_DEL 0x7F      // Delete
#define ASCII_SP  0x20

#define CONSOLE_SPACE 0x0720
#define CONSOLE_DEFAULT_TEXT_ATTR 0x07

void console_init();

void console_clear();

void console_write(char *buf, uint32_t count);



#endif
