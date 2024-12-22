#ifndef LSTDARGS_H_
#define LSTDARGS_H_


typedef char * va_list; // char *的使用使得 ++ 和 += 都是变化一个字节

#define va_start(ap, parmN) (ap = (va_list)&parmN + sizeof(parmN))
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap = (va_list)0)


#endif