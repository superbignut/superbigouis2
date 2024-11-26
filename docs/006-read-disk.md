[硬盘读写]()
  这里要补充一些关键字解释：

[柱面-盘面-扇区]()

  C-C-S: cylinder-Head-Sector 柱面-磁头-扇区
  L-B-A: logical-block-Address



[IDE-ATA-PIO-MODE](https://wiki.osdev.org/ATA_PIO_Mode)

> According to the ATA specs, PIO mode must always be supported by all ATA-compliant drives as the default data transfer mechanism.

> PIO mode uses a tremendous amount of CPU resources because every byte of data transferred between the disk and the CPU must be sent through the CPU's IO port bus (not the memory). On some CPUs, PIO mode can still achieve actual transfer speeds of 16MB per sec, but no other processes on the machine will get any CPU time.

> However, when a computer is just beginning to boot, there are no other processes. So PIO mode is an excellent and simple interface to utilize during bootup until the system goes into multitasking mode.

> Please note that this article deals with what are now styled PATA hard disks, as opposed to SATA hard disks.


这里指出， ATA 是一种硬盘的读写模式，也叫做 PATA

> The ATA disk specification is built around an older specification called ST506. With ST506, each disk drive was connected to a controller board by two cables -- a data cable, and a command cable. The controller board was plugged into a motherboard bus. The CPU communicated with the controller board through the CPU's IO ports, which were directly connected to the motherboard bus.

ATA 硬盘是插在主板上的， 最初是使用两个总线控制，数据总线和控制总线， CPU 通过IO端口与控制器通信

> What the original IDE specification did was to detach the disk controller boards from the motherboard, and stick one controller onto each disk drive, permanently. When the CPU accessed a disk IO port, there was a chip that shorted the CPU's IO bus pins directly onto the IDE cable -- so the CPU could directly access the drive's controller board. The data transfer mechanism between the CPU and the controller board remained the same, and is now called PIO mode. (Nowadays, the disk controller chips just copy the electrical signals between the IO port bus and the IDE cable, until the drive goes into some other mode than PIO.)

但是IDE标准将控制器集成到每一块硬盘上， 大差不差

> There is only one wire dedicated to selecting which drive on each bus is active. It is either electrically "high" or "low", which means that there can never be more than two devices operational on any ATA bus. They are called the master and the slave devices, for no particular reason. The terms 'master' and 'slave' have largely been abandoned as they inaccurately portray the master drive as having some kind of superiority over the slave drive, or that the latter is dependent on the master. However, these terms will be used in this document. The functionality of the master and slave drives is almost completely identical. There is a special IO port bit that allows a driver to select either drive as the target drive for each command byte.

从 bochsrc 中可以看到：

      ata0: enabled=true, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
      ata0-master: type=disk, path="build/master.img", mode=flat
      ata0-slave: type=none
      ata1: enabled=true, ioaddr1=0x170, ioaddr2=0x370, irq=15
      ata1-master: type=none
      ata1-slave: type=none
      ata2: enabled=false
      ata3: enabled=false

可以看到主要有两个ata， 分别是ata0 和ata1， 每个ata又分master 和 slave，在osdev 中也有指出：

> There is only one wire dedicated to selecting which drive on each bus is active. It is either electrically "high" or "low", which means that there can never be more than two devices operational on any ATA bus. They are called the master and the slave devices, for no particular reason. The terms 'master' and 'slave' have largely been abandoned as they inaccurately portray the master drive as having some kind of superiority over the slave drive, or that the latter is dependent on the master. However, these terms will be used in this document. The functionality of the master and slave drives is almost completely identical. There is a special IO port bit that allows a driver to select either drive as the target drive for each command byte.

应该是有一个控制位来选择 master 和 slave 这两个相同的ata硬盘的


> Current disk controller chips almost always support two ATA buses per chip. There is a standardized set of IO ports to control the disks on the buses. The first two buses are called the Primary and Secondary ATA bus, and are almost always controlled by IO ports 0x1F0 through 0x1F7, and 0x170 through 0x177, respectively (unless you change it). The associated Device Control Registers/Alternate Status ports are IO ports 0x3F6, and 0x376, respectively. The standard IRQ for the Primary bus is IRQ14 and IRQ15 for the Secondary bus.

disk 的读写芯片一般支持两个ata， 但是从bochsrc 中看到暂时只是用了ata0，具体的区别体现在cpu的io端口上


> All the ATA commands that use LBA addressing require "absolute" LBAs (ie. the sector offset from the very beginning of the disk -- completely ignoring partition boundaries). At first glance, it might seem most efficient to store the LBA values in this same format in your OS. However, this is not the case. It is always necessary to validate the LBAs that are passed into your driver, as truly belonging to the partition that is being accessed. It ends up being smartest to use partition-relative LBA addressing in your code, because you then never need to test if the LBA being accessed is "off the front" of your current partition. So you only need to do half as many tests. This makes up for the fact that you need to add the absolute LBA of the beginning of the current partition to every "relative" LBA value passed to the driver. At the same time, doing this can give you access to one additional LBA address bit.

这里的意思是每一个sector 是512个字节， 这里对应的应该就是 CCS 中的 确定了柱面和盘面（磁头）之后的扇区的大小，很显然，外圆空间要更大一些但是并没有利用好


> All current BIOSes have standardized the use of the IDENTIFY command to detect the existence of all types of ATA bus devices ... PATA, PATAPI, SATAPI, SATA.

> There are two other nonstandard techniques that are not recommended. The first is to select a device (then do a 400ns delay) then read the device's Status Register. For ATA devices that are not "sleeping", the RDY bit will always be set. This should be detectable, just so long as you have already tested for float (where all the bits are always set). If there is no device, then the Status value will be 0. This method does not work for detecting ATAPI devices -- their RDY bit is always clear (until they get their first PACKET command).

这里似乎提到了rdy位的作用,似乎和设备层面的ready相关


[out指令]()
> Copies the value from the second operand (source operand) to the I/O port specified with the destination operand
(first operand). The source operand can be register AL, AX, or EAX, depending on the size of the port being
accessed (8, 16, or 32 bits, respectively); the destination operand can be a byte-immediate or the DX register.
Using a byte immediate allows I/O port addresses 0 to 255 to be accessed; using the DX register as a source
operand allows I/O ports from 0 to 65,535 to be accessed.

这里的感觉就是虽然叫做端口port, 但是就是一个寄存器, 只不过是和外设有关的寄存器, 而且被限制了 只有ax 寄存器能用

更细节的介绍参考[ATA标准手册](http://ebook.pldworld.com/_eBook/ATA%20spec/ATA7_Spec.pdf)，比如第6章的指令介绍，

    + 6.16节 IDENTIFY DEVICE 0xEC
    + 6.35节 READ SECTORS 0x20
    + 6.67节 WRITE SECTORS 0x30
    
    卷二中更是提到的400ns的出处，还有很多状态转移框图，如果深入学习的话，都需要看一下

    下面是master硬盘驱动的端口号，范围是： 0x1f0 - 0x1f7 
    + 0x1f0 数据寄存器 16位
    + 0x1f1 r:错误寄存器， 暂不使用 w: 特征寄存器
    + 0x1f2 要读写的扇区的数量
    + 0x1f3 ~ 0x1f5 LBA的情况下是扇区号的前24位：0-23 分别是 low-mid-high
    + 0x1f6 
      + 0 ~ 3 LBA扇区号的24-27位
      + 4 drive-number : 0->master ; 1 -> slave 用来选择硬盘
      + 6 :  0 -> chs; 1->lba 用来选择方式，如果是lba，指令则可以使用28和48两种
      + 5,7: 固定是 1
    + 0x1f7
      + 写入的情况，也就是命令寄存器：
        + 0xEC : 识别硬盘
        + 0x20 : 读硬盘
        + 0x30 : 写硬盘
      + 读取的情况，也就是状态寄存器：, 所以只考虑最少的情况下,只读 DRQ和BUSY位即可
        + 0 ERR
        + 1,2 ： 0
        + 3 DRQ  -> 0 读到0表示数据准备完毕,可以下一次读取
        + 4 SRV
        + 5 DF 0 
        + 6 RDY ->1 # 这一位的功能暂时不太明白是什么?,似乎这一位是标志 设备是否ready 的意思
        + 7 BUSY ->0 读到0 表示命令完成 

    代码写完后有一些疑问：
       1. nop指令
       > This instruction performs no operation. It is a one-byte or multi-byte NOP that takes up space in the instruction stream but does not impact machine context, except for the EIP register.

       但是具体会耗时多少的细节没有提及,而是google的说法都是不建议用nop作为延迟，做延迟的话也是1/f级别的延迟

       2. 什么时候需要nop     

       OSDEV上的说法是，每次发送一个指令后，都需要等待一段时间：
       >Which means that a drive select may always happen just before a status read. This is bad. Many drives require a little time to respond to a "select", and push their status onto the bus. 

       3. 硬盘是怎么判断我成功读了一个字节，并准备下一个字节的
       
       这里也没有找到，这个应该也是在ATA标准中，而且就比如在read的循环中:
                
            .check_read_state:  ; 选择扇区后要延迟一下
                nop
                nop
                nop
                in al, dx ; 0x1f7
                and al, 0b1000_1000
                cmp al, 0b0000_1000
                jnz .check_read_state

            mov ax, 0x100   ; 把数据读到0x1000
            mov es, ax
            mov di, 0
            mov dx, 0x1f0

            read_loop:          ;读取数据
                nop
                nop
                nop
                in ax, dx
                mov [es:di], ax
                add di, 2
                cmp di, 512
                jnz read_loop
            xchg bx, bx
        
    读取数据之前即使没有确认数据准备完毕，也可以进行写一个字的读取，这个时序猜测是硬件规范中规定的，并且这里如果硬盘响应的比cpu的频率慢，肯定的是会出错的



虽然测试了一下把主引导扇区读到了0x8c00的内存, 和07c00比对了也没什么问题,但是硬盘读写里面还是问题多多,很多地方都说不太清, osdev 的文档也要多看看才行



[55aa-aa55]()


很多次用到这个数字，分配内存的时候可以是 

            [org 0x1000]
            db 0x55, 0xaa
            dw 0xaa55

第一种方式，虽然存的时候，是小地址 0x55， 大地址 0xaa， 但是如果读出 0x1000所在位置的一个字节的话，由于cpu是小端，所以取出的数字还是0xaa55

第二种方式更直接，直接存一个字 0xaa55, 同样由于是小端， 0x55被放到小地址，0xaa被放到大地址

<!-- 
    [org 0x7c00]

    mov ax, 3
    int 0x10
    xchg bx, bx
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00




    mov si, hello_str
    call real_printf    

    ; 把硬盘的第0个扇区(512B) 读到 0x8c00

    mov edi, 0x8c00
    mov ecx, 0      ; 第0 个扇区
    mov bl, 0x1     ; 就读1个

    call read_disk
    xchg bx, bx
halt:
    jmp halt



read_disk:
    pushad          ;eax, ecx, ebx, edx, esp, ebp, esi, edi 这里如果是16位的栈的话，32位寄存器会压两次
    push es         ; 中间会把es 置0, 所以先存了一下
    
    ;               从硬盘中读取数据到内存中
    ; 0:edi         读取数据，在内存中存放的位置 edi
    ; ecx           读取的原始数据在硬盘中的扇区位置-lba, 也就是第几个扇区
    ; bl            扇区数量

    mov dx, 0x1f2 
    mov al, bl      ; 读写扇区的数量
    out dx, al      ; out 指令将al 写入对应端口寄存器

    mov al, cl      ; ecx 低位放到对应端口
    mov dx, 0x1f3   ; low
    out dx, al

    shr ecx, 8      ; ecx 右移8位, 中8位放到对应端口
    mov al, cl      
    mov dx, 0x1f4   ; mid
    out dx, al

    shr ecx, 8      ; ecx 右移8位, 高8位放到对应端口
    mov al, cl
    mov dx, 0x1f5   ; high
    out dx, al

    shr ecx, 8
    and cl, 0b0000_1111         ; 取出0~3位是 lba的最高位
    mov al, 0b1110_0000
    or al, cl                   ;; 567 位全是1 , 表示 lba28
    mov dx, 0x1f6               ;master + lba_high_4
    out dx, al

    mov al, 0x20                ; read sectors command 0x20
    mov dx, 0x1f7
    out dx, al                  ; 放到w:指令寄存器io

    xor ecx, ecx                ; 清空
    mov cl, bl                  ; cl = bl = 读取扇区的数量

    .read:
        push cx                 ; cx was changed in .read_sector
        call .wait_sector       ; wait every sector read. 这里相当于以扇区为单位进行读取
        call .read_sector
        pop cx                  ; 因为cx 要作为loop的index位, 所以要先存起来
        loop .read

    pop es
    popad
    ret

    .wait_sector:
        mov dx, 0x1f7
        .check_read_state:          
            nop
            nop
            nop
            in al, dx               ; 把 0x1f7 r:状态寄存器中的值 读出来
            and al, 0b1000_1000
            cmp al, 0b0000_1000     ; DRQ 表示数据准备完成 BUSY表示指令执行结束
            jnz .check_read_state   
        ret


    .read_sector:           ; 每次读取一个扇区 512个字节
        mov dx, 0x1f0       ; 把16位数据拿出来
        mov cx, 256         ; loop one sector
        mov ax, 0
        mov es, ax          ; es set 0

        .read_loop:         ;读取数据
            nop
            nop
            nop
            in ax, dx               ; port = dx read port:dx to ax.
            mov [es:edi], ax        ; read to es:edi, 一次是两个字节
                                    ; 如果不进入保护模式的话，这个edi只能从0x0000 加到 0xffff 也就是 65535 / 512 = 128
                                    ; 最多为 128个扇区
            add edi, 2              ; edi
            loop .read_loop         ; loop 会减少cx
        ret

real_printf:
    ; si用于存放字符串首地址， 字符串用0表示结束
    mov cx, 0
    mov ds, cx
    mov ah, 0x0e
    
.next:
    mov byte al, [ds:si]
    cmp al, 0  
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

print:
    mov ax, 0
    mov ds, ax
    mov ax, 0xb800
    mov ds, ax
    mov byte [ds:0], 'H'
    iret

hello_str:
    db 'H', 'E', 'L', 'L', 'O', 0

    times 510 - ($-$$) db 0
    db 0x55, 0xaa


 -->