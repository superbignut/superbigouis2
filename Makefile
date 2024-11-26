# env: windows-Cmder.exe not windows-powershell


.PHONY: bochs, clean

bochs: build/master.img
# 启动bochs
	bochsdbg -q -f ./bochsrc 

build/master.img: build/boot.bin build/loader.bin
# 
ifeq ("$(wildcard build/master.img)", "")
# 创建硬盘镜像	
	bximage -q -func=create -hd=16M -imgmode=flat -sectsize=512 $@
endif
#   把boot.bin写512个字节到img的第0个扇区
	dd if=build/boot.bin of=$@ bs=512 count=1 
#   把loader.bin 写 4 个512字节 到img中，跳过前两个0，1扇区，从第2个扇区开始写 
	dd if=build/loader.bin of=$@ bs=512 count=4 seek=2 

build/%.bin: src/%.asm
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm $< -o $@


clean:
	rm -r build/
	
	
	