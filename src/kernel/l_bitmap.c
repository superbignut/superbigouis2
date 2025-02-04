#include "l_bitmap.h"
#include "l_string.h"
#include "l_assert.h"

/// @brief 将 bits length offset 赋值给 map
/// @param map 
/// @param bits 
/// @param length 
/// @param offset 
void bitmap_make(bitmap_t *map, char *bits, uint32_t length, uint32_t offset)
{
    map->bits = bits;
    map->length = length;
    map->offset = offset;
}

/// @brief 将 bits 全取为零， 再赋值给 map
/// @param map 
/// @param bits 
/// @param length 
/// @param offset 
void bitmap_init(bitmap_t *map, char *bits, uint32_t length, uint32_t offset)
{
    memory_set(bits, 0, length);
    bitmap_make(map, bits, length, offset);
}

/// @brief 检测 map 的某一位是否 为 1
/// @param map 
/// @param index 
/// @return 
bool bitmap_check(bitmap_t *map, uint32_t index)
{
    assert(index >= map->offset);               //  index 要大于 偏移

    uint32_t idx = index - map->offset;

    uint32_t bytes = idx / 8;

    uint32_t bits = idx % 8;
    
    assert(bytes < map->length);                //  小于缓冲区长度

    return (map->bits[bytes] & (1 << bits));
}

/// @brief 设置 map 的某一位
/// @param map 
/// @param index 
/// @param value 
void bitmap_set(bitmap_t *map, uint32_t index, bool value)
{
    assert(value == 0 || value == 1);

    assert(index >= map->offset);

    uint32_t idx = index - map->offset;

    uint32_t bytes = idx / 8;

    uint32_t bits = idx % 8;

    if(value == 1)
    {
        map->bits[bytes] |= (1 << bits);
    }
    else
    {
        map->bits[bytes] &= ~(1 << bits);
    }
}

/// @brief 从位图中找到 可用的 count 个 "连续" 内存页， 返回 index 
/// @param map 
/// @param count 
/// @return 
int bitmap_scan(bitmap_t *map, uint32_t count)
{
    int start = EOF;                                //  连续地址的开始，用于返回

    // uint32_t bits_to_check = map->length * 8;    //  总共的位数

    // uint32_t next_bit = 0;                       //  迭代 参数

    uint32_t bits_already_find = 0;                 //  已经找到的空闲位    

    size_t i = 0;                                   //  迭代参数

    for(i = 0; i < map->length * 8; ++i)
    {
        //  遍历所有位
        if(bitmap_check(map, map->offset + i) == 0)
        {
            //  空闲
            bits_already_find += 1;
        }
        else
        {
            //  不连续则清空， 重新找
            bits_already_find = 0;
        }

        if(bits_already_find == count)
        {
            //  找到了 count 个连续, 修改开始地址
            start = i - count + 1;
            break;
        }
    }

    if(start == EOF)
    {
        //  没找到, 返回 -1
        return -1;
    }

    while(bits_already_find--)
    {
        //  找到后 bitmap 置 1
        // next_bit--;
        bitmap_set(map, map->offset + i, 1);
    }

    return start + map->offset;                 //  返回 连续空闲页的开始 index 
}

#include "l_printk.h"

/// @brief 
void bitmap_test()
{

    bitmap_t map;

    #define len 2

    uint8_t buf[len];

    bitmap_init(&map, buf, len, 0);

    for(int i = 0; i < 18; ++i)
    {
        int idx = bitmap_scan(&map, 1);

        printk("%d\n", idx);
    }
}