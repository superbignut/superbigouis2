大致的思路就是

+ 32 位 的前 10 位 用于 去 页目录中 找一个 表项， 中间的20位 对应一个页大小的页表
+ 32 位 的中间 10 位 用于 去 页表中 再找一个 表项，中间的20位 对应一个页
+ 32 位 的最后 12 位 用于 去 页中 找对应的字节

#### 最终分页:


```cpp
#define KERNEL_PAGE_DIR 0x1000                  //  把loader 的地址覆盖

/// 内核页表
static uint32_t KERNEL_PAGE_TABLE[] = {
  0x2000,
  0x3000,
};
```

#### 映射结果:


```cpp
    0x0000000000001000-0x00000000007fffff -> 0x000000001000-0x0000007fffff
    0x00000000ffc00000-0x00000000ffc01fff -> 0x000000002000-0x000000003fff
    0x00000000fffff000-0x00000000ffffffff -> 0x000000001000-0x000000001fff
```

即 0 - 8MB 为内核代码部分, 第 0 个页触发中断

#### 注意

    0x1000 的位置原本用来存放 loader, 因此 paging_init 之后的代码不能再使用 loader 中的数据 比如 内存检测的结果, loader 中的 GDT 等



简易版的核心代码如下：

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


#### 在简易版的基础上,调整 内核页目录 和 内核页表 的位置, 并增加一个可以方便修改 页目录\页表 自己的索引项

```cpp
#define KERNEL_PAGE_DIR 0x1000                  //  把loader 的地址覆盖

/// 内核页表
static uint32_t KERNEL_PAGE_TABLE[] = {
  0x2000,
  0x3000,
};


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

    XBB;

    printk("#### PAGING INIT...\n");
}
```
以上的代码的结果如下, 用编号 1 2 3 分别解释:


1. 内核页表

        0x0000000000001000-0x00000000007fffff -> 0x000000001000-0x0000007fffff

    首先从 0x1000 开始的原因是因为 代码中跳过了第0页, 如果把 continue 注释掉 就是从第0 到 0x7fffff 也就是 8MB的内存

2. 页表修改

        为了方便的修改页表和页目录, 页目录最后一个 页表项指向 页目录 的所在页, 这也就导致了: 页目录也被当作一个 页表来看待, 从而导致了
        0x00000000ffc00000 这个线性地址, 最高10位 是 1 , 中间10位 是 0 , 进而 综合给出的页索引就是 0x1000 (页目录)页表的第一个表项的内容,也就是0x2000
        进而, 偏移地址是 0 , 最终指向的 就是 0x2000 这个页表的第一个字节

3. 页目录 修改
        修改页目录 也是同理, 0x00000000fffff000 前20位全是 1, 最终指向的页索引还是 0x1000, 进而可以进行修改,
        总结以下就是, 这个 页目录 即时 页目录 也是页表
```cpp
    /*
        <bochs:2> info tab
        cr3: 0x000000001000
        0x0000000000001000-0x00000000007fffff -> 0x000000001000-0x0000007fffff
        0x00000000ffc00000-0x00000000ffc01fff -> 0x000000002000-0x000000003fff
        0x00000000fffff000-0x00000000ffffffff -> 0x000000001000-0x000000001fff
    */
```