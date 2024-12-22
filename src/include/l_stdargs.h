/*
    目前只支持 int 类型参数， 如果把type 换成 char* 会更好用
*/
#ifndef LSTDARGS_H_
#define LSTDARGS_H_


typedef char * va_list; // char *的使用使得 ++ 和 += 都是变化一个字节

#define va_start(ap, parmN) (ap = (va_list)&parmN + sizeof(parmN))

#define va_arg(ap, type) (*(type *)((ap += sizeof(char *)) - sizeof(char *)))      // 这里考虑到对齐的话，type 的使用是错误的


#define va_end(ap) (ap = (va_list)0)


#endif