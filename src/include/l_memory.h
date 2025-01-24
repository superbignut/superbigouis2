#ifndef LMEMORY_H_
#define LMEMORY_H_

#include "l_types.h"

#define PAGE_SIZE 0x1000                        //  页的大小
#define MEMORY_BASE_ADDR 0x100000               //  内存起始基地址 1M 以外的地址

#define ARDS_VALID 1                            //  有效的地址
#define ARDS_RESERVED 2                         //  无效的地址

#define IDX(addr) (((uint32_t)(addr)) >> 12)                      //  页表索引，右移12位
#define PAGE(idx) ((uint32_t)(idx) << 12)                         //  页的开始地址
#define ASSERT_PAGE(addr) assert(((addr) & 0xfff) == 0)             //  检验是否是页地址，低 12位为0

/*
    Address Range Descriptor Structure 
  + 0	    BaseAddrLow		Low 32 Bits of Base Address  是返回的低地址 32位
  + 4	    BaseAddrHigh	High 32 Bits of Base Address 是返回的高地址 32位
  + 8	    LengthLow		Low 32 Bits of Length in Bytes 是可用内存的长度 以字节位单位 低32
  + 12	  LengthHigh		High 32 Bits of Length in Bytes 是可用内存的长度 以字节位单位 高32
  + 16	  Type		    Address type of  this range.  类型如下：
*/

typedef struct ards_ptr
{
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} _packed ards_ptr;

void memory_map_init();

#endif

