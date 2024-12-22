            ap = (va_list)&parmN + sizeof(parmN)

指向 v 的地址 + parmM 个字节，因为栈的地址向低增加, 所以prN作为第一个参数，会在最小的地址

+ (va_list)&parmN 会返回变量的低地址也就是下图的prN_low 
栈中变量存放的是低还是高地址可以查看（char*）&cnt+0 （char*）&cnt+1 （char*）&cnt+2 （char*）&cnt+3 

+ sizeof(parmN) 则会得到第一个参数的low地址

假设prn占4个字节， ap是int

addr  0    1    2    3    4    5    6    7    8 ...
      prN_low  prN_high   ap1_low  ap1_high   ap2     ap3     ap4 



所以这里一直挺模糊的地方就是 char* p;  p++; p+=2;
p++ 需要根据 指针的类型 具体执行++指向下一个值得操作
p+=2 也是在类型的基础上执行的加法，而不是 绝对的地址+2

当然当p的类型是char 的时候，都是+1 但是如果是其他类型 则不同

```cpp
#ifndef LSTDARGS_H_
#define LSTDARGS_H_


typedef char * va_list;

#define va_start(ap, parmN) (ap = (va_list)&parmN + sizeof(parmN))
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap = (va_list)0)


#endif
```


+ 测试代码
```cpp
void test_args(int cnt, ...){

    int a = 1;
    int *p = &a;
    p++;
    p+=2;
    va_list args;
    va_start(args, cnt);

    int arg;

    while(cnt--){
        arg = va_arg(args, int);

    }

    va_end(args);

}


test_args(5, 1 , 0xaa, 0x55, 2, 0);
```


