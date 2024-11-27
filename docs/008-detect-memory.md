[内存检测]()


对 int 0x15, ax=0xe820 和 Address Range Descriptor Structure 的介绍只有这一个[网站](http://www.uruk.org/orig-grub/mem64mb.html)，是从osdev中跳转过去的, 但不太清楚权威性和正确性

> Real mode only. This call returns a memory map of all the installed RAM, and of physical memory ranges reserved by the BIOS. The address map is returned by making successive calls to this API, each returning one "run" of physical address information. Each run has a type which dictates how this run of physical address range should be treated by the operating system. 

这里说要不断的调用，然后可以不断的返回，不太理解。

> If the information returned from INT 15h, AX=E820h in some way differs from INT 15h, AX=E801h or INT 15h AH=88h, then the information returned from E820h supersedes what is returned from these older interfaces. This allows the BIOS to return whatever information it wishes to for compatibility reasons.

上文中指出，int 0x15 是bios内存检测函数不断演变的终极版，而其他的那些函数可能是为了兼容性就没有被废除。

+ Input: 
  + EAX	Function Code	E820h
  + EBX	Continuation	Contains the "continuation value" to get thenext run of physical memory.  This is the value returned by a previous call to this routine.  If this is the first call, EBX must contain zero.
  
  + ES:DI	Buffer Pointer	Pointer to an  Address Range Descriptor	structure which the BIOS is to fill in.
  + ECX	Buffer Size	The length in bytes of the structure passed	to the BIOS.  The BIOS will fill in at most ECX bytes of the structure or however much of the structure the BIOS implements.  The minimum size which must be supported by both the BIOS and the caller is 20 bytes.  Future implementations may extend this structure.
  + EDX	Signature	'SMAP' -  Used by the BIOS to verify the caller is requesting the system map
  information to be returned in ES:DI.
	
+ 输入:
 + EAX 固定的 0xE820

    + EBX 每次都使用上层调用之后的返回值，初始化是0
     
    + ES：DI 指向一个输出地址吗, bios 会把数据写过去

    + ECX 指定了最大的字节数，bios 和调用者需要支持的最小是20字节
    
    + EDX 验证的签名 SMAP字母的ascii
      
+ Output:
  + CF Carry Flag   Non-Carry - indicates no error
  + EAX	Signature	'SMAP' - Signature to verify correct BIOS revision.
  + ES:DI Buffer Pointer	Returned Address Range Descriptor pointer. Same value as on input.
  + ECX	Buffer Size	    Number of bytes returned by the BIOS in the address range descriptor.  The minimum size	structure returned by the BIOS is 20 bytes.
  + EBX	Continuation	Contains the continuation value to get the next address descriptor.  The actual significance of the continuation value is up to the discretion of the BIOS.  The caller must pass the continuation value unchanged as input to the next iteration of the E820 call in order to get the next Address Range    Descriptor.  A return value of zero means that this is the last descriptor.  Note that the BIOS indicate that the last valid descriptor has been returned by either returning a zero as the continuation value, or by returning carry.

+ 输出:
  + CF 标志是否发生错误
  + EAX 把字符串又返了回来
  + ES:DI 不变
  + ECX 返回填入的实际字节数
  + EBX 最开始是0，然后每次调用bios会返回一个数，用于下次调用，当是最后一次调用时，EBX会返回0， 或者报错了，体现在CF上，返回cf=1

+ Address Range Descriptor Structure 对返回的结构进行了描述
  + 0	    BaseAddrLow		Low 32 Bits of Base Address  是返回的低地址 32位
  + 4	    BaseAddrHigh	High 32 Bits of Base Address 是返回的高地址 32位
  + 8	    LengthLow		Low 32 Bits of Length in Bytes 是可用内存的长度 以字节位单位 低32
  + 12	  LengthHigh		High 32 Bits of Length in Bytes 是可用内存的长度 以字节位单位 高32
  + 16	  Type		    Address type of  this range.  类型如下：

+  Type : 分别是被OS使用的和未使用的
     + 1       AddressRangeMemory      This run is available RAM usable by the operating system. 可用的内存
     + 2       AddressRangeReserved    This run of addresses is in use or reserved by the system, and must not be used by the operating system. 不可用的内存
     + else    Undefined		           是未定义的
     
+ Assumptions and Limitations 列出了一些int0x15的局限性，暂不展开
    
  + 某些特殊的地址不会被返回
  + 某些地址应该由OS来进行检测也不返回
    
+  这里在写'SMAP'的时候，如果直接写 
 +  mov eax, 'SMAP' 实际上move的是0x50414d53,也就是编译器把S放在了低位，因此int会触发CF报错
 +  mov eax, 0x534d4150 得到的与上面的相反，是正确的写法

**内存检测就里是最绕的**


+ jc 指令  
  > Jump short if carry (CF=1). 


通过检查 memory_detect_ards_buffer 可以看到内存检测的结果：
; 我的检测结果如下：type=1为可用区域

    ; 0x0_0000      len:0x9f000       type=1

    ; 0x9_f000      len:0x1000        type=2

    ; 0xe_8000      len:0x18000       type=2

    ; 0x10_0000     len:0x01ef_0000   type=1

    ; 0x1ff_0000    len:0x1_0000      type=3

    ; 0xfffc_0000   len:0x4_0000      type=3 

; 0x10_0000     l:0x1ef_0000    type=1 不到32MB的内存空间最大

; 所以bochs虚拟机里总共有两块内存可以使用






   + 内存检测代码如下:

            check_memory:

                mov ax, 0
                mov es, ax              ;es = 0

                xor ebx, ebx            ;ebx = 0

                mov edx, 0x534d4150     ;edx = 'SMAP'

                mov di, ards_buffer     ;di指向一个地址
            .next:                      ;循环标记
                mov eax, 0xe820         ;eax e820
                mov ecx, 20             ;只能写20
                int 0x15                ;中断调用

                jc .error               ;判断carry是否报错,cf是eflag的最低位

                add di, cx              ;地址移动20个
                inc word [ards_num]     ;统计数+1
                cmp ebx, 0              ;判断是不是0,是0结束,不用改
                jnz .next               ;循环
            
                mov cx, [ards_num]      ;看循环了几次
                mov si, 0               ;指针
                
            .show:                                  ;读取
                mov eax, [si + ards_buffer]         ;只读了低32位,也就是4个字节
                mov ebx, [si + ards_buffer + 8]     ;length
                mov edx, [si + ards_buffer + 16]    ;type
                add si, 20                          ;每次移动20个字节,读取数据
                loop .show                          ;循环读取
            .error:
                jmp $

            ards_num:
                dw 0
            ards_buffer: 

        有个问题是,在读取ards数据的时候,只读取了底32位,这里其实理解一下可以知道,32位能表示的数字其实是很恐怖的,以地址来说,32位是4G的内存指针,内存检测应该也检测不到这么大的范围之外吧暂时,在以后说不定会用的到高32位


