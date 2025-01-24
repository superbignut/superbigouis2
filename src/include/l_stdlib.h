#ifndef LSTDLIB_H_
#define LSTDLIB_H_
#include "l_types.h"


#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

void delay(uint32_t count);

// void hang(); 不喜欢这个函数

uint8_t bcd_to_bin(uint8_t bcd_data);

uint8_t bin_to_bcd(uint8_t bcd_data);

uint32_t div_round_up(uint32_t num, uint32_t size);

#endif