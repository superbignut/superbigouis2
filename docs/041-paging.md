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


#### 042-快表

这里只涉及到一个 当页表被修改后 需要取消对应的快表的命令：

```cpp
static void disable_tlb(uint32_t vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr): "memory");         //  memory 表示内存会被修改
}

```

这里是参考 osdev [tlb](https://wiki.osdev.org/TLB)


#### 043-bitmap

位图的代码主要围绕 bitmap 结构体展开：

```cpp

typedef struct bitmap_t
{
    uint8_t *bits;      //  位图指针（缓冲区）

    uint32_t length;    //  位图缓冲区数组 长度， 以字节为单位

    uint32_t offset;    //  index 减去 offset 后作为位图索引
} bitmap_t;

int bitmap_scan(bitmap_t *map, uint32_t count);

```

+ 首先 bits 用来存放位图指针的初始地址，也即是用每一位来代表一个页是否空闲

+ length 用来标记 bits 有多少个字节长， 如果用 vector<int> bitmap 来表示 其实就是 len(bitmap)

+ offset 其实有点特别，假设有一段 bits 的位图缓冲区，如果我想另起一个 bitmap 其实就可以 再新建一个 bitmap 但是
  设定一个 offset 从而允许一定的偏移量， 也即 同一段地址，但是不同的 offset，bitmap_scan 扫描就会找到不同的结果

+ 很显然 上面的 offset 的表述 是错的，下面的代码是在 memory_map_init 中 初始化 物理内存页数组 后面 初始化 虚拟内存位图的代码


```cpp
    //  前面为初始化物理内存数组，接下来位 初始化虚拟内存页-位图， 8位一个字节
    uint32_t len = (IDX(KERNEL_MEMORY_SIZE) - IDX(MEMORY_BASE_ADDR)) / 8;                       //  (IDX(8M) - IDX(1M)) / 8 = 224 ，8 位一个字节

    uint32_t offset =  IDX(MEMORY_BASE_ADDR);                                                   //  前 1M的页 不考虑， 设置偏移

    bitmap_init(&kernel_map, (char *)KERNEL_BITMAP_ADDR, len, offset);                          //  offset 的用处就在于跳过 某些不需要考虑的字节
    
    int tmp = bitmap_scan(&kernel_map, memory_map_pages_used);                                  //  为物理内存数组分配虚拟内存页 
                                                                                                //  也就是最开始的两页 返回 256 即 IDX(MEMORY_BASE_ADDR)
    assert(tmp == IDX(MEMORY_BASE_ADDR));                                                       //  分配结果 原则上就是 offset
```

    因此可以看到 offset 真正的用处在于，就是为了和全局页表编号对齐，比如要跳过 1MB 内存的页 也就 0x100 个页，但是我在外部访问的时候，比如 check 和set 函数：

    ```cpp
        static uint32_t reset_page(bitmap_t *map, uint32_t addr, uint32_t count)
        {
            ASSERT_PAGE(addr);                  //  有效页首地址
            uint32_t index = IDX(addr);         //  全局页号

            for(int i = 0; i < count; ++i)
            {
                assert(bitmap_check(map, index + i) == 1);      //  全局页号
                bitmap_set(map, index + i, 0);                  // 全局页号
            }
        }
    ```

    时，仍然可以使用全局 的页IDX 来进行操作，将具体的转换留在 bitmap 的操作内部


其实，虚拟页的位图 和 物理内存数组 还是有一点区别的，

+ 物理内存数组 是用来标记 32MB / 4KB = 8192 个 物理页 的使用情况的标记位
    + 这里记录下 物理内存页的 2 个页的计算
    + 32MB / 4KB 也就是 2^13 = 8K = 8192， 每个页中 的一个字节用来 标记这个页的占用的话，一页可以表示 4K个
      因此 8K / 4K = 2 也就是 memory_map_pages_used = 2 的原因
      
+ bitmap 暂时是用来 标记 0-8MB 的内存页 的使用情况的标记