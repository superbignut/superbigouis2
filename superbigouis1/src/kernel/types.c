#include "ltos/types.h"
#include "stdio.h"

typedef struct descriptor
{
    unsigned short limit_low;       // 长度
    unsigned int base_low : 24;     // 基地址
    unsigned char type : 4;         // 段类型
    unsigned char segment : 1;      // 0为代码段、数据段，1为系统段
    unsigned char DPL : 2;          // 0~3 访问权限
    unsigned char present : 1;      // 是否在内存中
    unsigned char limit_high : 4;   // 长度
    unsigned char available : 1;    // 0
    unsigned char long_mode : 1;    // 0
    unsigned char big : 1;          // 32位置1
    unsigned char granularity : 1;  // limit粒度 - 表示字节，1表示4KB
    unsigned char base_high;        // 基地址

} _packed descriptor; 


int main(){
    descriptor td;
    printf("%d\n", sizeof(u8));
    printf("%d\n", sizeof(u16));
    printf("%d\n", sizeof(u32));
    printf("%d\n", sizeof(u64));
    printf("%d\n", sizeof(descriptor));

    return 0;
}