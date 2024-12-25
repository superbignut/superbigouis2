#include "l_printk.h"


static char buf[1024];


/// @brief print in kernel. 在 vsprintf的基础上，在屏幕上打印出来
/// @param fmt 
/// @param  
/// @return 
int printk(const char *fmt, ...){

    va_list args;
    int i;

    va_start(args, fmt);
    
    i = vsprintf(buf, fmt, args);

    va_end(args);

    console_write(buf, i);      // 打印
    
    return i;
}


