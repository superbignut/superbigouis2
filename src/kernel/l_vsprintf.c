/*
    @ return : Number of characters written if successful (not including the terminating null character) or a negative value if an error occurred.
    + vsprintf 
    + sprintf
*/
#include "l_stdio.h"



/// @brief 从 va_list 中不断匹配 fmt 中的 "%*" 格式化字符，写到 buf 中
/// @param buf 
/// @param fmt 
/// @param args 
/// @return 
int vsprintf(char *buf, const char *fmt, va_list args){
    




}


/// @brief 依赖于 vsprintf， 把格式化的字符串写进buf
/// @param buf 
/// @param fmt 
/// @param  
/// @return 
int sprintf(char *buf, const char *fmt, ...){
    va_list args;
    int i;
    
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}