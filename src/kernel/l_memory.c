#include "l_memory.h"
#include "l_types.h"
#include "l_os.h"
#include "l_debug.h"
#include "l_assert.h"
#include "l_printk.h"

uint32_t memory_base = 0;                       //  基地址
uint32_t memory_size = 0;                       //  地址长度
uint32_t total_pages = 0;                       //  总页数
uint32_t free_pages = 0;                        //  空闲页数

#define used_pages (total_pages - free_pages)   // 已用页数

/// @brief 内存初始化代码，从 start 中跳入， 输入参数在 loader 中保存
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
        //  printk("%x, %x\n", memory_base, memory_size);
        assert(memory_base == MEMORY_BASE_ADDR);        //  检验最大内存起始地址

        total_pages = IDX(memory_size) + IDX(memory_base);
        
        free_pages = IDX(memory_size);

        //  printk("%d, %d\n", total_pages, free_pages);
        //  while(True);
    }
    else
    {
        panic("memory_init error.\n");
    }
}