#include "l_memory.h"
#include "l_types.h"
#include "l_os.h"
#include "l_debug.h"
#include "l_assert.h"
#include "l_printk.h"
#include "l_stdlib.h"

static uint32_t memory_base = 0;                        //  基地址
static uint32_t memory_size = 0;                        //  地址长度

static uint32_t start_page = 0;                         //  被分配的起始页号, 不参与更新
static uint32_t total_pages = 0;                        //  总页数
static uint32_t free_pages = 0;                         //  空闲页数

// #define used_pages (total_pages - free_pages)           // 已用页数

/// @brief 读取内存检测 buffer，从 start 中跳入， 输入参数在 loader 中传递入栈
/// @param magic 魔数， 作为一个校验值
/// @param ards_num_ptr 这个参数是 ards 数量的 指针
/// @param ards_buffer_ptr 这个参数是 ards 存放地址位置的 指针
void memory_init(uint32_t magic, uint32_t ards_num_ptr, uint32_t ards_buffer_ptr)
{
    uint32_t _num;
    ards_ptr *_ptr;
    size_t i;

    assert(sizeof(ards_ptr) == 20);         //  结构体大小检验

    if(magic == OS_MAGIC)                   //  魔数检验
    {
        _num = *(uint32_t *)ards_num_ptr;
        _ptr = (ards_ptr *)ards_buffer_ptr;

        for(i = 0; i < _num; ++i, ++_ptr)
        {
            uint64_t a = _ptr->addr;
            //  printk 只能打印 32位 的参数， 根源于可变参数只能用 32 位 的指针， 所以打印前修改为 32位
            //  printk("%x, %x, %x\n", (uint32_t)_ptr->addr, (uint32_t)_ptr->size, _ptr->type);

            //  这里找到区域最大的可用内存
            if(_ptr->type == ARDS_VALID && _ptr->size > memory_size)
            {
                memory_base = (uint32_t)_ptr->addr;
                memory_size = (uint32_t)_ptr->size;
            }
        }
        assert(memory_base == MEMORY_BASE_ADDR);        //  检验最大内存起始地址

        total_pages = IDX(memory_size + memory_base);
        
        free_pages = IDX(memory_size);

        printk("#### MEMORY INIT...\n");
    }
    else
    {
        panic("memory_init error.\n");
    }
}

static uint8_t *memory_map_array;               //  物理内存数组, 放在最初的页中
static uint32_t memory_map_pages_used;          //  已被 物理内存数组 占用的页数 >= 1 页， 类似于页目录的概念

/// @brief 初始化物理内存-物理页
void memory_map_init()
{   
    memory_map_array = (uint8_t *)memory_base;
    memory_map_pages_used = div_round_up(total_pages, PAGE_SIZE);               //  每一个页存放 PAGE_SIZE 个字节，计算需要的页数

    free_pages -= memory_map_pages_used;                                        //  更新空闲页数
    memory_set(memory_map_array, 0, memory_map_pages_used * PAGE_SIZE);         //  物理内存数组 清零
    
    start_page = IDX(MEMORY_BASE_ADDR) + memory_map_pages_used;                 //  第一个可用的页
    // printk("%d %d\n", total_pages, start_page);

    for(size_t i = 0; i < start_page; ++i)                                      //  小于 start_page 的页 标记为已用
    {
        memory_map_array[i] = 1;                                                //  物理内存数组 占用标记
    }
    printk("#### MEMORY MAP INIT...\n");
}

/// @brief 申请页表, 返回页首地址
/// @return 
static uint32_t get_page()
{
    for(size_t i = start_page; i < total_pages; ++i)                    //  找到空闲页
    {
        if(memory_map_array[i] == 0)
        {
            memory_map_array[i] = 1;
            --free_pages;
            uint32_t page = (uint32_t)i << 12;
            return page;
        }
    }
    panic("page error.\n");
}

/// @brief 释放页表
/// @param addr 
/// @return 
static void put_page(uint32_t addr)
{
    ASSERT_PAGE(addr);                      //  检验是否是页地址

    uint32_t idx = IDX(addr);               //  页索引

    assert(idx >= start_page && idx < total_pages);

    assert(memory_map_array[idx] >= 1);     //  检验当前引用

    memory_map_array[idx] -= 1;             //  

    if(memory_map_array[idx] == 0)          //  如果减到 0 
    {
        // printk("yes\n");
        free_pages++;
    }
}


/* void mem_test()
{
    uint32_t pgs[12];
    for(int i = 0; i < 10; ++i)
    {
        pgs[i] = get_page();
    }
    for(int i = 0; i < 10; ++i)
    {
        put_page(pgs[i]);
    }
} */