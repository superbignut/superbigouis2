#ifndef LBITMAP_H_
#define LBITMAP_H_

#include "l_types.h"


/// @brief 位图 结构体
typedef struct bitmap_t
{
    uint8_t *bits;      //  位图指针（缓冲区）

    uint32_t length;    //  位图缓冲区数组 长度， 以字节为单位

    uint32_t offset;    //  index 减去 offset 后作为位图索引
} bitmap_t;

void bitmap_make(bitmap_t *map, char *bits, uint32_t length, uint32_t offset);

void bitmap_init(bitmap_t *map, char *bits, uint32_t length, uint32_t offset);

bool bitmap_check(bitmap_t *map, uint32_t index);

void bitmap_set(bitmap_t *map, uint32_t index, bool value);

int bitmap_scan(bitmap_t *map, uint32_t count);

#endif