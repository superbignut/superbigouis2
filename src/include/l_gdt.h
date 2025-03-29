/**
 * @file l_gdt.h
 * @author bignut
 * @brief Gdt header file.
 * @version 0.1
 * @date 2025-03-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LGDT_H_
#define LGDT_H_

#include "l_types.h"

#define GDT_SIZE 127    /**< max num of gdt_size is 2**13, which is depends on segment selector. */

/**
 * @brief This is the element/gdt_item in global-descriptor-table.
 * 
 * @details GDT表中的表项
 */
typedef struct segment_descriptor      
{
    uint16_t limit_low;
    uint32_t base_low : 24;
    uint32_t segment_type : 4;      /**< 如果这32个字节不全都使用 uint32_t 会出问题(bit-map in struct_c ) */          
    uint32_t descriptor_type : 1;
    uint32_t DPL : 2;
    uint32_t segment_present : 1;
    uint8_t  limit_high : 4;
    uint8_t  AVL : 1;
    uint8_t  L : 1;
    uint8_t  D_B : 1;
    uint8_t  granularity : 1;
    uint8_t  base_high;
} _packed segment_descriptor;


/**
 * @brief Segment_selector + offset is used in protected-mode to locate address in memory.
 * 
 * @details jmp (0x1 << 3): 0x10000
 * 
 */
typedef struct segment_selector        
{
    uint16_t RPL : 2;
    uint16_t TI : 1;
    uint16_t index : 13;    /**< Which gdt-item in gdt-table */
} _packed segment_selector;


/**
 * @brief Used by lgdtr to locate and move gdt's base addr and limit into gdtr-register.
 * 
 */
typedef struct gdtr_content            
{
    uint16_t limit;
    uint32_t base_addr;
} _packed gdtr_content;

/**
 * @brief Gdt_c init function.
 * 
 */
void gdt_init();

#endif