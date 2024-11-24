[http://www.chrysocome.net/dd](http://www.chrysocome.net/dd)

+ 示例1
        dd if=boot.bin of=master.img bs=512

[bs 参数]()

> The default block size is 512 which will work for most files and devices but the copy will be a lot faster if you use a larger block size. For example a floppy disk read with bs=1k count=1440 takes almost twice as long than if you use bs=1440k count=1. Don't make the block size too large because windows will run out of memory.
bs 参数的含义按照d-f-w 的解释, 具体的理解就是，每次搬运的字节数，不指定count 参数的话，默认应该是要copy输入文件的所有字节

