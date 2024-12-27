+ 这一节的主要工作就是把原来在 loader 中写的 gdt 拷贝到新的位置，然后 在把新的位置加载到 gdtr 寄存器

问题大多出在前面，


```cpp
typedef struct segment_descriptor{      //  GDT表中的表项
    uint16_t limit_low;
    uint32_t base_low : 24;
    uint32_t segment_type : 4;          //  如果这32个字节不全都使用 uint32_t 会出问题
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

```

最开始 只有 base_low 我给到了 uint32_t， 后面其余的我用的都是 uint8_t， 但是发现写出来的总的字节数不对 不是8个字节，并且问题就出在这个32位的类型不一致上，他们没有放在一起，后来把 uint32_t 一共32个位的都写成一个类型就ok了
