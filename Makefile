# env: windows-Cmder.exe not windows-powershell

BUILD := ./build
SRC := ./src

BUILD_BOOT := $(BUILD)/boot
SRC_BOOT :=$(SRC)/boot

SRC_KERNEL := $(SRC)/kernel
BUILD_KERNEL := $(BUILD)/kernel

ENTRY_POINT := 0x10000

CFLAG := -m32
CFLAG += -fno-builtin 			# 不需要gcc的内置函数
CFLAG += -nostdinc 				# 不需要标准头文件
CFLAG += -fno-pic 				# 不需要位置无关的代码 
CFLAG += -fno-pie 				# 不需要位置无关的可执行程序
CFLAG += -nostdlib 				# 不需要标准库
CFLAG += -fno-stack-protector 	# 不需要栈保护
CFLAG += -v
DEBUG := -g 					# 调试信息	
# 栈保护和位置无关的代码应该是会在原有代码上插入一些修改信息，所以禁用掉
# 头文件 和 内置函数 不需要也禁用掉
INCLUDE := -I./src/include # 头文件

CC = gcc

.PHONY: bochs clean qemu qemug test

bochs: $(BUILD)/master.img
# 启动bochs
# @echo $(CFLAG)
	bochsdbg -q -f ./bochsrc 

qemu: $(BUILD)/master.img
	qemu-system-i386 -m 32M -boot c -hda $<

qemu-debug: $(BUILD)/master.img
	qemu-system-i386 -s -S -m 32M -boot c -hda $<	

# boot.bin -> boot.asm
# loader.bin -> loader.asm
# system.bin -> kernel.bin -> start.o -> start.asm
# 
$(BUILD)/master.img: $(BUILD_BOOT)/boot.bin \
					 $(BUILD_BOOT)/loader.bin \
					 $(BUILD_KERNEL)/system.bin 

#$(BUILD_KERNEL)/system.map

ifeq ("$(wildcard $(BUILD)/master.img)", "")
# 创建硬盘镜像	
	bximage -q -func=create -hd=16M -imgmode=flat -sectsize=512 $@
endif
#$(shell mkdir -p $(dir $@))	
#   把boot.bin写512个字节到img的第0个扇区	[0, 0x200)
	dd if=$(BUILD_BOOT)/boot.bin of=$@ bs=512 count=1 	
#   把loader.bin 写 4 个512字节 到img中，跳过前两个0,1扇区，从第2个扇区开始写 0x400 [0x400, 0xC00]
	dd if=$(BUILD_BOOT)/loader.bin of=$@ bs=512 count=4 seek=2 
#	把system.bin 写 8 个512字节 到img中， 跳过前10个扇区（0-9），从第10个扇区开始写 [0x1400, ...) 注意不是0x2000
	dd if=$(BUILD_KERNEL)/system.bin of=$@ bs=512 count=200 seek=10


###################################################### 0. 编译boot 和loader
$(BUILD_BOOT)/%.bin: $(SRC_BOOT)/%.asm
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm $< -o $@

###################################################### 4. 把 i386pe 的代码段单独拿出来
$(BUILD_KERNEL)/system.bin: $(BUILD_KERNEL)/kernel.bin
	objcopy -O binary $< $@	

###################################################### 3. asm 和 c 链接到一起
$(BUILD_KERNEL)/kernel.bin: $(BUILD_KERNEL)/start.o \
							$(BUILD_KERNEL)/main.o	\
							$(BUILD_KERNEL)/l_io.o	
# 这里链接到了汇编和c # 并制定了代码段的位置 # 并且完成静态链接
	ld -m i386pe -static $^ -o $@ -Ttext $(ENTRY_POINT)

###################################################### 1. 从 asm 编译出 .o
$(BUILD_KERNEL)/%.o: $(SRC_KERNEL)/%.asm
# @echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm -f win32 $(DEBUG) $< -o $@
###################################################### 2. 从 .c 编译出 .o
$(BUILD_KERNEL)/%.o: $(SRC_KERNEL)/%.c
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAG) $(DEBUG) $(INCLUDE) -c $< -o $@
###################################################### 5. 这里拿出了符号表，不知道干什么
$(BUILD_KERNEL)/system.map: $(BUILD_KERNEL)/kernel.bin
	nm $< | sort > $@

clean:
	rm -r $(BUILD)/

test.s: .\src\test\test.c
	$(CC) -m32 -S $< \
	-fno-asynchronous-unwind-tables \
	-mpreferred-stack-boundary=2 \
	-fno-ident \
	-o $@
#
#-mpreferred-stack-boundary=2  是让栈 以4 个字节进行对齐