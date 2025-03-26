+ 0x0000

+ 0x1000 内核页目录 / loader.asm was readed by boot.asm(from disk to memory:0x1000)

+ 0x2000 内核页表 1

+ 0x3000 内核页表 2

+ 0x4000 内核 虚拟内存位图 bitmap

+ 0x10000 内核栈底， loader 中设定

+ 0x10000 kernel.bin kernel 代码开始

+ 0x29000 kernel.bin kernel 代码结束 0x10000 + 200 * 0x512 = 0x29000

+ 0x100000 物理内存数组 memory_map_array

+ 0x800000 内核内存结束
