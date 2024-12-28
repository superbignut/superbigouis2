#ifndef LOS_H_
#define LOS_H_

#define OS_MAGIC 20241130

void kernel_init(); // 

/// @brief 最开始进到 c 里面在屏幕上打印的代码
///        虽然没什么用了，但不舍得删掉
static void inline hello_c_code(){
    
    char msg[] = "Running C Code...";
    
    char *video = (char *)0xb8000;

    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }
}


#endif