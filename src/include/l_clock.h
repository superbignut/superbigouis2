#ifndef LCLOCK_H_
#define LCLOCK_H_

#define PIT_8253_CHANNEL_0 0x40
#define PIT_8253_CHANNEL_1 0x41
#define PIT_8253_CHANNEL_2 0x42

#define PIT_8253_MODE_CMD_REG 0x43      //  只写


#define OSCILLATOR 1193182              //  这个是 8253 counter 衰减的频率

#define HZ 20                           //  HZ 越大，Rate Generator 产生的频率越大 

#define CLOCK_COUNTER (OSCILLATOR / HZ) //  这个数值不能大于 16位的 65535， 因此 HZ 的最大值为 18
                                        
                                        //  由于.h文件没有加入 makefile 修改完 需要clean 才能看到效果
void clock_init();

#endif