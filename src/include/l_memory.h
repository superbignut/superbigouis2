/**
 * @file l_memory.h
 * @author bignut
 * @brief 
 * @version 0.1
 * @date 2025-03-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LMEMORY_H_
#define LMEMORY_H_

#include "l_types.h"
#include "l_bitmap.h"

#define PAGE_SIZE 0x1000                        //  页的大小
#define MEMORY_BASE_ADDR 0x100000               //  内存起始基地址 1M 以外的地址
#define INDEX_NUM_PER_PAGE (0x1000 / 4)         //  一页中 页表项的数目

#define ARDS_VALID 1                            //  有效的地址
#define ARDS_RESERVED 2                         //  无效的地址


/// 内核页目录
#define KERNEL_PAGE_DIR 0x1000                  //  把loader 的地址覆盖

/// 内核页表
static uint32_t KERNEL_PAGE_TABLE[] = {
  0x2000,
  0x3000,
};

#define KERNEL_BITMAP_ADDR 0x4000                    //  位图开始地址

#define KERNEL_PAGE_NUM (sizeof(KERNEL_PAGE_TABLE) / sizeof(uint32_t))

#define KERNEL_MEMORY_SIZE (0x400000 * KERNEL_PAGE_NUM)           //  内核页表能映射的内存， 一个页 4MB
                                                                                          //  也即 内核使用的内存 0 - 8MB
#define IDX(addr) (((uint32_t)(addr)) >> 12)                      //  页表索引，右移12位
#define PAGE(idx) ((uint32_t)(idx) << 12)                         //  页的开始地址

#define IDX_DIR(addr) (((uint32_t)addr) >> 22)                    //  页目录编号 最高10位
#define IDX_TABLE(addr) ((((uint32_t)addr) >> 12) & 0x3ff)        //  页表编号 中间10位

#define ASSERT_PAGE(addr) assert(((addr) & 0xfff) == 0)           //  检验是否是页地址，低 12位为0

/*
  
*/


/**
 * @brief Structure used by memory-detected consequence.
 * 
 * @details Address Range Descriptor Structure 
 * + 0	    BaseAddrLow		Low 32 Bits of Base Address       是返回的低地址 32位
 * + 4	    BaseAddrHigh	High 32 Bits of Base Address      是返回的高地址 32位
 * + 8	    LengthLow		  Low 32 Bits of Length in Bytes    是可用内存的长度 以字节位单位 低32
 * + 12	    LengthHigh		High 32 Bits of Length in Bytes   是可用内存的长度 以字节位单位 高32
 * + 16	    Type		      Address type of  this range.      类型如下：
 */
typedef struct ards_ptr
{
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} _packed ards_ptr;



/** @struct page_entry
 *  
 * @brief page_entry_structure used as pge-item or pte-item.
 */
typedef struct page_entry
{
  uint32_t present : 1;         //  1 表示 存在
  uint32_t read_write : 1;      //  1 表示 可写
  uint32_t user_super : 1;      //  1 表示 超级用户
  uint32_t pwt : 1;             //  1 表示 直写 
  uint32_t pcd : 1;             //  1 表示 进制缓存
  uint32_t accessed : 1;        //  1 表示 访问过
  uint32_t dirty : 1;           //  1 表示 写过
  uint32_t pat : 1;             //  0 

  uint32_t global : 1;          //  1 表示 全局使用
  uint32_t ignored : 3;         //  
  uint32_t index : 20;          //  页索引
} _packed page_entry;

void memory_map_init();

uint32_t get_cr3();

void set_cr3(uint32_t pde);

void paging_init();

uint32_t alloc_kernel_page(uint32_t count);

void free_kernel_page(uint32_t vaddr, uint32_t count);

#endif

