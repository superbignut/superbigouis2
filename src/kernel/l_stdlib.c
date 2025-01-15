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

uint8_t bcd_to_bin(uint8_t bcd_data)
{
    return (bcd_data & 0x0f) + 10 *((bcd_data & 0xf0) >> 4);
}

uint8_t bin_to_bcd(uint8_t bcd_data)
{
    return (bcd_data % 10) + ((bcd_data / 10) <<  4);
}