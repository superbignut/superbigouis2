/*
    参考 [CRT](http://www.osdever.net/FreeVGA/vga/crtcreg.htm#0E)
*/


#ifndef LIO_H_
#define LIO_H_

#include "l_types.h"


#define CRT_ADDR_REG_PORT 0x3d4
#define CRT_DATA_REG_PORT 0x3d5
#define Input_Output_Address_Select_PORT 0x3cc          // 判断是不是0x3d4 最后一位需要是1

#define CRT_CUSOR_HIGH_INDEX 0xe
#define CRT_CUSOR_LOW_INDEX 0xf


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
