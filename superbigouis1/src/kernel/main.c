#include "ltos/ltos.h"

int magic = LT_MAGIC;

char msg[] = "Hello ltos!";

char buf[1024];


void kernel_init(){
    char  * video = (char *) 0xb8000;
    
    for(int i=0 ;i< sizeof(msg); ++i){
        video[i * 2] = msg[i];
    }   
}

