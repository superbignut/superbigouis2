/*
    参考 [CRT](http://www.osdever.net/FreeVGA/vga/crtcreg.htm#0E)
*/


#ifndef LIO_H_
#define LIO_H_

#include "l_types.h"

/// 用于判断CRT地址数据寄存器的0x3X4的X
#define Miscellaneous_Output_Register 0x3CC          

/// CRT 数据、地址寄存器
#define CRT_ADDR_REG_PORT 0x3d4
#define CRT_DATA_REG_PORT 0x3d5

#define CRT_START_ADDR_HIGH_INDEX 0xC
#define CRT_START_ADDR_LOW_INDEX 0xD
#define CRT_CUSOR_HIGH_INDEX 0xE
#define CRT_CUSOR_LOW_INDEX 0xF


/// VGA图形 数据、地址寄存器
#define VGA_GRAPHICS_ADDR_REG_PORT 0x3CE
#define VGA_GRAPHICS_DATA_REG_PORT 0x3CF

/// 用于查看VGA的内存位置和大小
#define Memory_Map_Select_INDEX 0x06   


/// @brief 给定端口， 读8位
/// @param port 
/// @return 
uint8_t read_byte_from_vga(uint16_t port);

/// @brief 给定端口， 读16位
/// @param port 
/// @return 
uint16_t read_word_from_vga(uint16_t port); 


/// @brief 给定端口，将8位 value 写入端口
/// @param port 
/// @param value 
void write_byte_to_vga(uint16_t port, uint8_t value);

/// @brief 给定端口，将16位 value 写入端口
/// @param port 
/// @param value 
void write_word_to_vga(uint16_t port, uint16_t value);

#endif
