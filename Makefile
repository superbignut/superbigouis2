# env: windows-Cmder.exe not windows-powershell

BUILD := ./build
SRC := ./src

BUILD_BOOT := $(BUILD)/boot
SRC_BOOT :=$(SRC)/boot

SRC_KERNEL := $(SRC)/kernel
BUILD_KERNEL := $(BUILD)/kernel

ENTRY_POINT := 0x10000


.PHONY: bochs, clean

bochs: $(BUILD)/master.img
# 启动bochs
	bochsdbg -q -f ./bochsrc 

# boot.bin -> boot.asm
# loader.bin -> loader.asm
# system.bin -> kernel.bin -> start.o -> start.asm
# 
$(BUILD)/master.img: $(BUILD_BOOT)/boot.bin \
					 $(BUILD_BOOT)/loader.bin \
					 $(BUILD_KERNEL)/system.bin \
					 $(BUILD_KERNEL)/system.map
# 
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

$(BUILD_BOOT)/%.bin: $(SRC_BOOT)/%.asm
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm $< -o $@

$(BUILD_KERNEL)/system.bin: $(BUILD_KERNEL)/kernel.bin
	objcopy -O binary $< $@

$(BUILD_KERNEL)/kernel.bin: $(BUILD_KERNEL)/start.o
	ld.gold -m elf_i386 -static $^ -o $@ -Ttext $(ENTRY_POINT)

$(BUILD_KERNEL)/%.o: $(SRC_KERNEL)/%.asm
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $< -o $@

$(BUILD_KERNEL)/system.map: $(BUILD_KERNEL)/kernel.bin
	nm $< | sort > $@

clean:
	rm -r $(BUILD)/
	
	
	