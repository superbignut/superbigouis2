#include "l_stdlib.h"


/// @brief 一段延迟
/// @param count 
void delay(uint32_t count){

    while(count--);
}

/// @brief 阻塞的代码
void hang(){

    while(True);
}