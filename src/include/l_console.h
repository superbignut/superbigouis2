/**
 * @file l_console.h
 * @author bignut
 * @brief Console include file.
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef LCONSOLE_H_
#define LCONSOLE_H_
#include "l_types.h"
#include "l_io.h"

#define MEM_ADDR_BASE 0xB8000   /**< VGA video-card first character address. */
#define MEM_ADDR_SIZE 0x8000    /**< 32k VGA_SIZE, there are some register to check size and addr of vga-card such as Memory_Map_Select_INDEX */ 
#define MEM_ADDR_END (MEM_ADDR_BASE + MEM_ADDR_SIZE)    /**< End addr of vga */

#define SCREEN_CHAR_WIDTH 80    /**< width read from vga-register */ 
#define SCREEN_CHAR_HEIGHT 25   /**< height read from vga-register */ 


#define SCREEN_WIDTH_BYTE_SIZE (SCREEN_CHAR_WIDTH * 2)      /**< byte-num-width */
#define SCREEN_HEIGHT_BYTE_SIZE (SCREEN_CHAR_HEIGHT * 2)    /**< byte-num-height */
#define SCREEN_SIZE_BYTE_SIZE (SCREEN_HEIGHT_BYTE_SIZE * SCREEN_WIDTH_BYTE_SIZE)   /**< byte-num-height*width */ 

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

#define CONSOLE_SPACE 0x0720    /**< NULL/Space Character */
#define CONSOLE_DEFAULT_TEXT_ATTR 0x07  /**< Default color of character*/

void console_init();

void console_clear();

void console_write(char *buf, uint32_t count);



#endif
