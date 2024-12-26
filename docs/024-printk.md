[linux-vsprintf.c](https://github.com/torvalds/linux/blob/master/lib/vsprintf.c)


格式化字符串总体格式：

        %[flags][width][.prec][h|l|L][type]

> The format string consists of ordinary byte characters (except %), which are copied unchanged into the output stream, and conversion specifications. Each conversion specification has the following format:

1. introductory % character.

2. (optional) 对齐和符号

    one or more flags that modify the behavior of the conversion:
    -: the result of the conversion is left-justified within the field (by default it is right-justified).

    +: the sign of signed conversions is always prepended to the result of the conversion (by default the result is preceded by minus only when it is negative).

    space: if the result of a signed conversion does not start with a sign character, or is empty, space is prepended to the result. It is ignored if + flag is present.

    #: alternative form of the conversion is performed. See the table below for exact effects otherwise the behavior is undefined.
    0: for integer and floating-point number conversions, leading zeros are used to pad the field instead of space characters. For integer numbers it is ignored if the precision is explicitly specified. For other conversions using this flag results in undefined behavior. It is ignored if - flag is present.
  

3. (optional) 最小宽度

    integer value or * that specifies minimum field width. The result is padded with space characters (by default), if required, on the left when right-justified, or on the right if left-justified. In the case when * is used, the width is specified by an additional argument of type int, which appears before the argument to be converted and the argument supplying precision if one is supplied. If the value of the argument is negative, it results with the - flag specified and positive field width (Note: This is the minimum width: The value is never truncated.).

4. (optional) 精度

    . followed by integer number or *, or neither that specifies precision of the conversion. In the case when * is used, the precision is 
    specified by an additional argument of type int, which appears before the argument to be converted, but after the argument supplying minimum field width if one is supplied. If the value of this argument is negative, it is ignored. If neither a number nor * is used, the precision is taken as zero. See the table below for exact effects of precision.

5. (optional) 修饰符

    length modifier that specifies the size of the argument (in combination with the conversion format specifier, it specifies the type of the corresponding argument).

6. conversion format specifier. 具体格式



测试了一下 c的循环语句的汇编代码：

```cpp

int main(){
    char *str;
    int a;
    for(str = "123"; *str!= 0; ++str){		
        ++a;
    }

    return 0;
}

```

```asm
	.file	"test.c"
	.text
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC0:
	.ascii "123\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	___main
	movl	$LC0, -4(%ebp)
	jmp	L2
L3:
	addl	$1, -8(%ebp)		； 这个是函数的体
	addl	$1, -4(%ebp)		； 这个是for 的第三个操作 这里是 ++str， 被放在了函数体的下面
L2:
	movl	-4(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L3
	movl	$0, %eax
	leave
	ret

```

并对比了一下 for 的判断条件中 写成 *str 编译出来也是一样的， 都是 	
		
		movzbl	(%eax), %eax	； 先取出一个字节
		testb	%al, %al		； 按位与自己， 只有全是0 的情况下，结果才是零 ，因此也就是 str 中的Null-terminated string 的结束标志了


const char ** fmt参数

这里的话因为想要实现的功能是字符串转整数，并且还要可以移动 fmt 指针， 因此如果只是使用 const char * 作为参数达不到想要的效果，移动的只是函数内部的指针

```cpp
void test(const char *fmt){
    fmt++;							//  这里不会修改调用位置的 fmt 指针
    return;
}


void test(const char **fmt){
    (*fmt)++;						//  对原 fmt 进行修改
    return;
}

```



这里我又试了一下之前的lnet 中的 sscanf函数，发现%hhx 和 % hx 和 %x 如果用 unsigned char 接受 确实是完全不同的结果

```cpp
#include <stdint.h>
#include <stdio.h>

int main(){
    char *a = "1A:2A";
    unsigned char b = 0;
    unsigned char c = 0;
    sscanf(a, "%hhx:%hhx", &b, &c);				//  可以把这里改成 hhx hx x 试一下, 输入读取问题多多，打印就没这么多事
    printf("%d %d\n", b, c);
    int d = 0;
    return 0;
}
```


找到了两个容易 报错 

		expression must be a modifiable lvalueC/C++(137)
的地方

	1. 一个是给指针赋值 而不是 给 *ptr 赋值
	
		console.cursor_ptr++ = ch; 

	2. 另一个是给 临时变量++ 

		((char *)console.cursor_pos)++; // 不管是前面++还是后面++都是错误的
