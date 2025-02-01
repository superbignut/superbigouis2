大致的思路就是

+ 32 位 的前 10 位 用于 去 页目录中 找一个 表项， 中间的20位 对应一个页大小的页表
+ 32 位 的中间 10 位 用于 去 页表中 再找一个 表项，中间的20位 对应一个页
+ 32 位 的最后 12 位 用于 去 页中 找对应的字节



核心代码如下：

```cpp

void paging_init()
{
    assert(sizeof(page_entry) == 4);                        //  检测页表项结构体

    page_entry *pde = (page_entry *)KERNEL_PAGE_DIR;        //  创建一个页目录
    memory_set(pde, 0, PAGE_SIZE);
    
    page_entry_init(&pde[0], IDX(KERNEL_PAGE_ENTRY));       //  初始化 页目录 的第0个 页表项 指向内核页表

    page_entry *pte = (page_entry *)KERNEL_PAGE_ENTRY;      //  内核页表
    memory_set(pte, 0, PAGE_SIZE);

    page_entry *entry;

    for(size_t tindex = 0; tindex < INDEX_NUM_PER_PAGE; ++tindex)        //  将第一个页全部赋值 总共映射1K个页 也即 4MB 的物理内存
    {
        entry = &pte[tindex];
        page_entry_init(entry, tindex);                                 //  把 tindex 号物理页 映射到 tindex 的 pte 中
        memory_map_array[tindex] += 1;                                  //  物理内存使用次数 +1
    }

    set_cr3((uint32_t)pde);

    enable_32bit_paging();
}

```


流程就是：
+ 创建页目录， 并指向页表
+ 创建页表 指向最开始的 1K 个页 也就是 4MB 的物理内存
+ cr3 赋值为 内核 pde
+ cr0 开启 内存映射