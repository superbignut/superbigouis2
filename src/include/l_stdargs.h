/*
    目前只支持 int 类型参数， 如果把type 换成 char* 会更好用
*/
#ifndef LSTDARGS_H_
#define LSTDARGS_H_

#include "l_types.h"

typedef char *va_list; // char *的使用使得 ++ 和 += 都是变化一个字节

// 返回可变参数的首地址
#define va_start(ap, fmt) (ap = (va_list)&fmt + sizeof(char *))                     // 指向可变参数的地址 + 一个指针的长度

// 每次移动首地址到下一个参数
#define va_arg(ap, type) (*(type *)((ap += sizeof(char *)) - sizeof(char *)))       // 这里考虑到对齐的话，type 的使用是错误的

// args指向空指针
#define va_end(ap) (ap = (va_list)0)


#endif