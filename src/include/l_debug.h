#ifndef L_DEBUG_H
#define L_DEBUG_H

void debugk(char *file, int line, const char *fmt, ...);

#define XBB asm volatile("xchg %bx, %bx")                                       //  bochs 断点

#define DEBUGK(fmt, args...) debugk(__FILE__, __LINE__, fmt, ##args)            //  第一次见这么用的， 把可变参数做一次传递

#endif