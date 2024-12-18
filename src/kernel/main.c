#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"

int magic = OS_MAGIC;



void hello_c_code(){
    char msg[] = "Running C Code...";
    char *video = (char *)0xb8000;
    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }
}


void kernel_init(){

    hello_c_code();
    console_init();
}