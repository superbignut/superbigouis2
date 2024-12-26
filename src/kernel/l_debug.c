#include "l_debug.h"
#include "l_stdargs.h"
#include "l_stdio.h"
#include "l_printk.h"

static char buf[1024];

/// @brief panic 去掉 spin 阻塞
/// @param file 
/// @param line 
/// @param fmt 
/// @param  
void debugk(char *file, int line, const char *fmt, ...){

    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    printk("FILE: %s LINE: %d %s", file, line, buf);

}