/**
 * @file l_memory.c
 * @author bignut
 * @brief Ards and pyhsics-addr-bit-map and paging, which is the most ESSENTIAL file/function in this os.
 * @version 0.1
 * @date 2025-03-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "l_memory.h"
#include "l_types.h"
#include "l_os.h"
#include "l_debug.h"
#include "l_assert.h"
#include "l_printk.h"
#include "l_stdlib.h"

static uint32_t memory_base = 0;                /**< base-addr which was init in ards-init function */    
static uint32_t memory_size = 0;                /**< size/num-addr which was init in ards-init function */    

static uint32_t start_page = 0;                 /**< first usable page start-physics-page-num for user-to-use which was init in memory_map_init function */

static uint32_t total_pages = 0;                /**< ALL-PAGE-DETRCTED. Althought it just is max-ards-size : base + size which was init in ards-init function */
static uint32_t free_pages = 0;                 /**< PAGE-SIZE-DETRCTED. Althought it just is max-ards-size : size which was init in ards-init function*/

bitmap_t kernel_bitmap;                         /**< bit-map global var used to sl kernel-phy-bit-map which was init in memory_map_init function*/

// #define used_pages (total_pages - free_pages)           // 已用页数

/// @brief 读取内存检测 buffer，从 start 中跳入， 输入参数在 loader 中传递入栈
/// @param magic 魔数， 作为一个校验值
/// @param ards_num_ptr 这个参数是 ards 数量的 指针
/// @param ards_buffer_ptr 这个参数是 ards 存放地址位置的 指针

/**
 * @brief Find the max-size ards-structure and update memory_base, memory_size, total_pages and free_pages.
 * 
 * @details All three params are passed from loader.asm and to start.asm and here.
 * 
 * @param magic top stack. which is a fixed number in this os.
 * 
 * @param ards_num_ptr mid stack. The num of usable address block.
 * 
 * @param ards_buffer_ptr bottom stack. Stucture-ptr of ards-detected.
 */
void ards_init(uint32_t magic, uint32_t ards_num_ptr, uint32_t ards_buffer_ptr)
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

    if(memory_size < KERNEL_MEMORY_SIZE)                //  内存检测的大小，不能小于内核的内存需求大小， 这里是 4MB * 内存页数
    {
        panic("max memory detected is smaller than kernel needed.\n");
    }
}

//  物理内存数组, 放在最初的页中，每个编号的字节 代表当前编号的页 被引用的次数

static uint8_t *memory_map_array;   /**> pyhsics memory map array, which use 8-bit to represent a pyh-page's state(how many times used). */

//  已被 物理内存数组 占用的页数 >= 1 页， 类似于页目录的概念

static uint32_t memory_map_pages_used;  /**>The num of pages used by memory_map_array to indicate all total_pages. */

/// @brief 初始化物理内存-物理页， 初始化虚拟页-位图

/**
 * @brief Compute the num of pages used by memory_map_array, update memory_map_pages_used, start_page...Todo
 * 
 * @details While memory_map_pages_used is calculated out, update memory_map_pages_used, free_pages, and memory_map_array
 *          Update memory_map_array From 0 to IDX(base_addr = 0x100_000 + memory_map_pages_used = 2), set to 1.
 *          For the reason that 8-bits is used as an item in memory_map_array, so PAGE_SIZE = 0x1000 = 4k = 2^12
 *          is also the memory_map_item_per_page.
 */
void memory_map_init()
{   
    memory_map_array = (uint8_t *)memory_base;

    uint32_t memory_map_item_per_page = PAGE_SIZE;                                              //  每一个页存放 PAGE_SIZE 个字节

    memory_map_pages_used = div_round_up(total_pages, memory_map_item_per_page);                //  计算需要的页数

    free_pages -= memory_map_pages_used;                                                        //  更新空闲页数
    memory_set(memory_map_array, 0, memory_map_pages_used * memory_map_item_per_page);          //  物理内存数组 清零
    
    start_page = IDX(MEMORY_BASE_ADDR) + memory_map_pages_used;                                 //  第一个可用的页
    // printk("%d %d\n", total_pages, start_page);

    for(size_t i = 0; i < start_page; ++i)                                                      //  小于 start_page 的页 标记为已用
    {
        memory_map_array[i] = 1;                                                                //  物理内存数组 占用标记
    }
    printk("#### MEMORY MAP INIT...\n");



    //  前面为初始化物理内存数组，接下来位 初始化虚拟内存页-位图， 8位一个字节
    uint32_t len = (IDX(KERNEL_MEMORY_SIZE) - IDX(MEMORY_BASE_ADDR)) / 8;                       //  (IDX(8M) - IDX(1M)) / 8 = 224 ，8 位一个字节

    uint32_t offset =  IDX(MEMORY_BASE_ADDR);                                                   //  前 1M的页 不考虑， 设置偏移

    bitmap_init(&kernel_bitmap, (char *)KERNEL_BITMAP_ADDR, len, offset);                       //  offset 的用处就在于跳过 某些不需要考虑的字节
    
    int tmp = bitmap_scan(&kernel_bitmap, memory_map_pages_used);                               //  为物理内存数组分配虚拟内存页 
                                                                                                //  也就是最开始的两页 返回 256 即 IDX(MEMORY_BASE_ADDR)
    assert(tmp == IDX(MEMORY_BASE_ADDR));                                                       //  分配结果 原则上就是 offset

    /*
        这里按照测试的代码来看 memory_map_pages_used = 2，因此分配了2个页后

        kernel_bitmap 从 0x102000 开始的 bitmap 为空闲位，一直到 0x7ff000

        一共 0x800 - 0x100 - 2 = 0x6fe 个 空闲的内核页
    */
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

/// @brief 返回 cr3寄存器的值
/// @return 
uint32_t get_cr3()
{
    asm volatile("movl %cr3, %eax");           // eax 存放返回值
}

/// @brief 设置 cr3 寄存器
/// @param pde 
void set_cr3(uint32_t pde)
{
    ASSERT_PAGE(pde);
    asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));         //  输入操作数部分， pde参数 赋值给 eax
                                                            //  正常应该用两个 %% 百分号来引用寄存器
}

/// @brief 开启分页
static void enable_32bit_paging()
{
    asm volatile(
        "movl %cr0, %eax\n"
        "orl $0x80000000, %eax\n"              //  cr0 最高位是 PG
        "movl %eax, %cr0"
    );
}


/// @brief 初始化页表项，把页索引赋予页表项
/// @param entry 
/// @param idx 
static void page_entry_init(page_entry *entry, uint32_t idx)
{
    memory_set(entry, 0, sizeof(page_entry));       //  页表项清零

    entry->present = 1;
    entry->read_write = 1;
    entry->user_super = 1;
    entry->index = idx;
}


/// @brief 初始化内存映射，配置页目录、页表， 将虚拟页 映射到 物理页上
void paging_init()
{
    assert(sizeof(page_entry) == 4);                        //  检测页表项结构体

    page_entry *pde = (page_entry *)KERNEL_PAGE_DIR;        //  创建一个页目录
    memory_set(pde, 0, PAGE_SIZE);

    uint32_t index = 0;                                     //  全局物理页的 index

    for(uint32_t idx_dir = 0; idx_dir < KERNEL_PAGE_NUM; ++idx_dir)
    {
        page_entry *pte = (page_entry *)KERNEL_PAGE_TABLE[idx_dir];                 //  遍历页表
        
        memory_set(pte, 0, PAGE_SIZE);

        page_entry_init(&pde[idx_dir], IDX(pte));                                   //  配置页目录

        page_entry *entry;

        for(size_t idx_table = 0; idx_table < INDEX_NUM_PER_PAGE; ++idx_table, ++index)      //  将第一个页全部赋值 总共映射1K个页 也即 4MB 的物理内存
        {
            
            entry = &pte[idx_table];                                                

            if(index == 0)                                                          //  0号页不参与映射, 对0 号页的访问会失败
            {
                memory_set(entry, 0, sizeof(page_entry));                           //  页表项清零
                continue;
                /*
                    If a paging-structure entry’s P flag (bit 0) is 0 or if the entry sets any reserved bit, the entry is used neither to refer-
                    ence another paging-structure entry nor to map a page. There is no translation for a linear address whose transla-
                    tion would use such a paging-structure entry; a reference to such a linear address causes a page-fault exception
                    (see Section 5.7).
                */
            }
                                                                                    //  下面两行为，映射物理页， 编号为 index 
            page_entry_init(entry, index);                                          //  把 tindex 号物理页 映射到 tindex 的 pte 中

            memory_map_array[index] += 1;                                           //  前1k个页的 物理内存使用次数 +1
        }
    }

    page_entry *last_entry = &pde[INDEX_NUM_PER_PAGE-1];                            //  页目录的最后一个页表项

    page_entry_init(last_entry, IDX(KERNEL_PAGE_DIR));                              //  指向自己

    set_cr3((uint32_t)pde);

    enable_32bit_paging();

    printk("#### PAGING INIT...\n");
    /*
        <bochs:2> info tab
        cr3: 0x000000001000
        0x0000000000001000-0x00000000007fffff -> 0x000000001000-0x0000007fffff
        0x00000000ffc00000-0x00000000ffc01fff -> 0x000000002000-0x000000003fff
        0x00000000fffff000-0x00000000ffffffff -> 0x000000001000-0x000000001fff
        左边是线性地址, 右边是物理地址
    */
}

/// @brief 返回页目录 的 首地址
/// @return 
static page_entry *get_pde()
{
    return (page_entry*)0xfffff000;     //  返回第0个页目录
}

/// @brief 返回地址所在页表 的 首地址
/// @param addr 
/// @return 
static page_entry *get_pte(uint32_t addr)
{
    return (page_entry*)(0xffc00000 | IDX_DIR(addr) << 12);
    /*
        0x0      - 0x3fffff 返回 0xffc00000
        0x400000 - 0x7fffff 返回 0xffc01000 
    */
}

/// @brief 返回虚拟地址 对应的页目录的 页表项
/// @param vvaddr 
/// @return 
static page_entry *get_vaddr_pde_entry(uint32_t vaddr)
{
    page_entry *pde = get_pde();

    return &pde[IDX_DIR(vaddr)];
}

/// @brief 返回虚拟地址 对应的页表的 页表项
/// @param vvaddr 
/// @return 
static page_entry *get_vaddr_pte_entry(uint32_t vaddr)
{
    page_entry *pte = get_pte(vaddr);

    return &pte[IDX_TABLE(vaddr)];
}

/// @brief 将虚拟地址对应的页目录的页表项 进行修改
/// @param vaddr 
/// @param table_index IDX 之后的编号
/// @return 
static page_entry *set_vaddr_pde_entry_with_index(uint32_t vaddr, uint32_t table_index)
{
    page_entry *dir_entry = get_vaddr_pde_entry(vaddr);
    page_entry_init(dir_entry, table_index);
}

/// @brief 将虚拟地址对应的页表的页表项 进行修改
/// @param vaddr 
/// @param paddr_index IDX 之后的编号
/// @return 
static page_entry *set_vaddr_pte_entry_with_index(uint32_t vaddr, uint32_t paddr_index)
{
    page_entry *table_entry = get_vaddr_pte_entry(vaddr);
    page_entry_init(table_entry, paddr_index);
}

/// @brief 无效指定虚拟地址的快表 (修改虚拟地址对应的页表之后执行)
/// @param vaddr 
static void disable_tlb(uint32_t vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr): "memory");         //  memory 表示内存会被修改

    /*

        https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html

        When not using an asmSymbolicName, use the (zero-based) position of the operand 
        in the list of operands in the assembler template. For example if there are two 
        output operands and three inputs, use ‘%2’ in the template to refer to the first
        input operand, ‘%3’ for the second, and ‘%4’ for the third.

        The "memory" clobber tells the compiler that the assembly code performs memory 
        reads or writes to items other than those listed in the input and output operands 
        (for example, accessing the memory pointed to by one of the input parameters). To
        ensure memory contains correct values, GCC may need to flush specific register 
        values to memory before executing the asm. Further, the compiler does not assume 
        that any values read from memory before an asm remain unchanged after that asm; 
        it reloads them as needed. Using the "memory" clobber effectively forms a read/write 
        memory barrier for the compiler.
    */
}

/// @brief 封装 bitmap， 从 bitmap_scan 得到的 index 返回 32位页首地址
/// @param map 
/// @param count 
/// @return 
static uint32_t scan_page(bitmap_t *map, uint32_t count)
{
    int index = bitmap_scan(map, count);        //  失败返回 -1

    if(index == -1)
    {
        panic("scan_page error!");
    }
    uint32_t addr = PAGE(index);                //  恢复页地址

    return addr;                                //  返回页地址
}

/// @brief 从 addr 开始回收 cont 的 map 页， addr 为有效 页首地址
/// @param map 
/// @param count 
/// @return 
static uint32_t reset_page(bitmap_t *map, uint32_t addr, uint32_t count)
{
    ASSERT_PAGE(addr);                  //  有效页首地址
    uint32_t index = IDX(addr);

    for(int i = 0; i < count; ++i)
    {
        assert(bitmap_check(map, index + i) == 1);
        bitmap_set(map, index + i, 0);
        assert(bitmap_check(map, index + i) == 0);
    }
}

/// @brief 分配 count 个连续内核页, 返回起始页的 首地址
/// @param count 
/// @return 
uint32_t alloc_kernel_page(uint32_t count)
{
    uint32_t addr = scan_page(&kernel_bitmap, count);
    return addr;
}

/// @brief 释放 count 个连续内核页
/// @param vaddr 起始页的首地址
/// @param count 
void free_kernel_page(uint32_t vaddr, uint32_t count)
{
    ASSERT_PAGE(vaddr);
    reset_page(&kernel_bitmap, vaddr, count);
}

/// @brief 测试 memory_map_init 初始化位图后 还能继续 分配多少个内核页， 对最大值 0x6fe 进行测试，并测试 free
void memory_test2()
{
    uint32_t *pages = (uint32_t *)(0x200000);
    uint32_t count = 0x6fe;                 //  这里最大是 0x6fe个页, 0x200 + 2 + 0x6fe = 0x800  
    for (size_t i = 0; i < count; i++)
    {
        pages[i] = alloc_kernel_page(1);
        printk("0x%x 0x%x\n", i, pages[i]);
    }
    for (size_t i = 0; i < count; i++)
    {
        free_kernel_page(pages[i], 1);
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

/// @brief 将一个大于 内存的虚拟地址映射到 一个物理地址上
///        更新快表并测试
void paging_test()
{
    XBB;

    uint32_t vaddr = 0x4000000;   // 64 MB 虚拟地址
    uint32_t paddr = 0x1400000;   // 20 MB 物理地址
    uint32_t pt = 0x900000;     // 页表

    /* page_entry *pde = get_pde();

    page_entry *dir_entry = &pde[IDX_DIR(vaddr)];

    page_entry_init(dir_entry, IDX(pt));       //  页目录 指向 页表 */


    set_vaddr_pde_entry_with_index(vaddr, IDX(pt));

    /* page_entry *pte = get_pte(vaddr);

    page_entry *table_entry = &pte[IDX_TABLE(vaddr)];

    page_entry_init(table_entry, IDX(paddr));     //  页表映射到物理页上 */

    set_vaddr_pte_entry_with_index(vaddr, IDX(paddr));

    
    XBB;


    char *ptr = (char *)(0x4000000);
    ptr[0] = 'a';

    XBB;

    set_vaddr_pte_entry_with_index(vaddr, IDX(0x1500000)); //  改变物理映射

    disable_tlb(vaddr);

    ptr[2] = 'b';

    XBB;
}