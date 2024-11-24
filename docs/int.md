[int 0x10中断参考](https://stanislavs.org/helppc/int_10-0.html)

+ int 0x10 整体是设置显示的模式，我们使用的是 ax = 0x03, 具体暂不深入了解
    AH = 00
	AL = 00  40x25 B/W text (CGA,EGA,MCGA,VGA)
	   = 01  40x25 16 color text (CGA,EGA,MCGA,VGA)
	   = 02  80x25 16 shades of gray text (CGA,EGA,MCGA,VGA)
	   = 03  80x25 16 color text (CGA,EGA,MCGA,VGA)

目前来说起到的作用就是 实现了清屏的功能