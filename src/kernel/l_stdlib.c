#include "l_stdlib.h"


/// @brief 一段延迟
/// @param count 
void delay(uint32_t count)
{
    while(count--);
}

/// @brief 阻塞的代码
void hang()
{
    while(True);
}

/// @brief bcd 转 二进制
/// @param bcd_data 
/// @return 
uint8_t bcd_to_bin(uint8_t bcd_data)
{
    return (bcd_data & 0x0f) + 10 *((bcd_data & 0xf0) >> 4);
}

/// @brief 二进制 转 bcd
/// @param bcd_data 
/// @return 
uint8_t bin_to_bcd(uint8_t bcd_data)
{
    return (bcd_data % 10) + ((bcd_data / 10) <<  4);
}

/// @brief num / size 向上取整
/// @param num 
/// @param size 
/// @return 
uint32_t div_round_up(uint32_t num, uint32_t size)
{
    return (num - 1) / size + 1;
}