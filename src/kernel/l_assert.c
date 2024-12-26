#include "l_assert.h"
#include "l_printk.h"


static char buf[1024];

/// @brief 代码阻塞
/// @param name 
static void spin(char *name){
    printk("--> Code was Fucked up by %s ...\n", name);
    while(True);    
}


/// @brief 不直接使用， 使用 assert 
/// @param exp 
/// @param file 
/// @param base 
/// @param line 
void assertion_failure(char *exp, char *file, char *base, int line){
    printk(
        "\n--> assert expression (%s) failed!\n"
           "--> file is: %s \n"
           "--> base is: %s \n"
           "--> line is: %d \n",
           exp, file, base, line);
    
    spin("assertion_failure()");

    // asm volatile("ud2");            //  error
}


/// @brief 比 assert 多了自定义的打印项
/// @param fmt 
/// @param  
void panic(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int i = vsprintf(buf, fmt, args);
    va_end(args);

    printk("--> #Panic! --> %s \n", buf);
    spin("panic()");

    // asm volatile("ud2");        //  error
}