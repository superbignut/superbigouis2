+ c语言代码：

```cpp

int add(int x, int y){
    int z = x + y;
    return z;
}
int main(){

    int a = 1;
    int b = 2;
    int c  = add(a, b);

    return 0;
}

```
+ makefile

```makefile
test.s: .\src\test\test.c
	$(CC) -m32 -S $< \
	-fno-asynchronous-unwind-tables \
	-mpreferred-stack-boundary=2 \
	-fomit-frame-pointer \
	-fno-ident \
	-o $@
```

+ 生成的汇编， 

比较有意思的是两个地方：
+ 第一是ip 存放的是当前即将要执行的指令的位置, 调用call 的时候会将下一条地址的位置入栈， 这里之所以比较有意思就是，因为函数的参数也是要入栈的，
所以，这个ip 是会压在参数的上面的

+ 第二个是函数参数入栈，从右向左一次入栈， 并且由于栈的增长的方向是向小地址增长，因此很多都是 subl 

```asm

	.file	"test.c"
	.text
	.globl	_add
	.def	_add;	.scl	2;	.type	32;	.endef
_add:
	subl	$4, %esp                ; 申请 4 个字节的栈 ，保存局部变量
	movl	8(%esp), %edx           ; 这里即跳过了 自己申请的局部变量，也跳过了 main函数的ip， 拿到第一个参数
	movl	12(%esp), %eax          ; 拿到第二个参数
	addl	%edx, %eax              ; 加法
	movl	%eax, (%esp)            ; 结果传递给局部变量的栈的位置
	movl	(%esp), %eax            ; 结果传给eax
	addl	$4, %esp                ; 栈恢复
	ret                             ; eip 恢复， 接着main 执行
	.def	___main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	subl	$20, %esp               ; 申请20个字节的栈的空间， 栈顶指针改变
	call	___main                 ; 不清楚
	movl	$1, 16(%esp)            ; 在栈顶指针向下偏移16字节的地方 放置局部变量 1 ， 其实这里也就是 申请的20个字节的栈的最底下的空间了
	movl	$2, 12(%esp)            ; 在栈顶指针向下偏移12字节的地方 放置局部变量 2
	movl	12(%esp), %eax          ; 把 参数 2 入栈到 指针向下偏移4字节的位置
	movl	%eax, 4(%esp)           ; 
	movl	16(%esp), %eax          ; 把 参数 1 入栈到 指针向下偏移0字节的位置
	movl	%eax, (%esp)            ;
	call	_add                    ; 调用函数 这里会将 下一条mov 入栈 ， 在add 的ret 时加载到eip寄存器
	movl	%eax, 8(%esp)           ; 把返回值 也是另一个局部变量 入栈到 指针向下偏移8字节的位置
	movl	$0, %eax                ; main 函数的返回值 是 0 传给eax 
	addl	$20, %esp               ; 栈的申请 结束 栈指针回去
	ret

```

``` debug console

    -exec display/8xw $sp # 这个命令可以从 debug console 中 help 查看， google 基本查不到
```