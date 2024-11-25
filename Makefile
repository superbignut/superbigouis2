# env: windows-Cmder.exe not windows-powershell


.PHONY: bochs, clean

bochs: build/master.img
# 启动bochs
	bochsdbg -q -f ./bochsrc 

build/master.img: build/boot.bin
# 
ifeq ("$(wildcard build/master.img)", "")
# 创建硬盘镜像	
	bximage -q -func=create -hd=16M -imgmode=flat -sectsize=512 $@
endif
	dd if=build/boot.bin of=$@ bs=512 count=1 


build/%.bin: src/%.asm
#@echo $(dir $@)
	$(shell mkdir -p $(dir $@))
	nasm $< -o $@


clean:
	rm -r build/
	
	
	