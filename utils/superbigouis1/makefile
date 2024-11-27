ENTRYPOINT := 0x10000

CFLAG := -m32
CFLAG += -fno-builtin # 不需要gcc的内置函数 such as : memcpy
CFLAG += -nostdinc # 不需要标准头文件
CFLAG += -fno-pic # 不需要位置无关的代码 
CFLAG += -fno-pie # 不需要位置无关的可执行程序
CFLAG += -nostdlib # 不需要标准库
CFLAG += -fno-stack-protector # 不需要栈保护
CFLAG := $(strip ${CFLAG}) # 删除结尾的换行，更简洁

DEBUG := -g # 调试信息

INCLUDE := -I./src/include # 头文件

.PHONY: bochs
bochs: build/master.img
#	@echo "$(dir $<)"
	bochs -q  

build/master.img: build/boot.bin \
			build/loader.bin \
			build/system.bin \
			build/system.map 
ifeq ("$(wildcard build/master.img)", "")
	bximage -func=create -hd=16M -imgmode=flat -sectsize=512 $@ -q
# 创建硬盘镜像
endif
	dd if=build/boot.bin of=$@ bs=512 count=1 conv=notrunc
# MBR 主引导扇区
	dd if=build/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc	
# 跳过前两个扇区 从0x400 * 16 bytes 开始写4 * 512 = 2048个字节进入磁盘
	dd if=build/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc
# 跳过前10个扇区，写200个扇区

# 似乎 build/kernel/%.o 有两个入口, 这里应该是会对依赖文件是否存在进行一个判断，存在才会执行
build/kernel/%.o: src/kernel/%.asm
	$(shell mkdir -p $(dir $@))
#	@echo "$(dir $@)"
	nasm -f elf32 $(DEBUG) $< -o $@

build/kernel/%.o: src/kernel/%.c
	$(shell mkdir -p $(dir $@))
#	@echo "$(dir $@)"
	gcc $(CFLAG) $(DEBUG) $(INCLUDE) -c $< -o $@

build/kernel.bin: build/kernel/start.o \
				  build/kernel/main.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext=$(ENTRYPOINT)
# -m elf_i386: Emulate the emulation linker. 链接生成32位的i386指令
# -static: Do not link against shared libraries. This is only meaningful
#          on platforms for which shared libraries are supported. 
# 使用-static 暂不理解，是为了不链接其他的动态库吗
#-Ttext=org: When creating an ELF executable, it will set the address 
# 			 of the first byte of the text segment.
# 将elf32的.o文件链接成 .bin可执行文件


build/system.bin: build/kernel.bin
	objcopy -O binary $< $@
# The GNU objcopy utility copies the contents of an object file to another. 
# objcopy can be used to generate a raw binary file by using an output target 
# of `binary' (e.g., use `-O binary'). When objcopy generates a raw binary file,
# it will essentially produce a memory dump of the contents of the input object 
# file. All symbols and relocation information will be discarded. The memory dump
# will start at the load address of the lowest section copied into the output file.
# 将 elf文件中的其他内容去掉


# 暂不理解
build/system.map: build/kernel.bin
	nm $< | sort > $@
# GNU nm lists the symbols from object files
# 将符号表 排序后输出到system.map中


test: build/kernel.bin

test2: build/kernel2.bin

build/%.bin: src/%.asm
	$(shell mkdir -p $(dir $@))
# 如果没有build目录，需要先创建出来
	nasm $< -o $@ 

.PHONY: clean
clean:

	rm ./build/ -rf
