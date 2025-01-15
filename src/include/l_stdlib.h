#ifndef LSTDLIB_H_
#define LSTDLIB_H_
#include "l_types.h"


void delay(uint32_t count);

// void hang(); 不喜欢这个函数

uint8_t bcd_to_bin(uint8_t bcd_data);

uint8_t bin_to_bcd(uint8_t bcd_data);

#endif