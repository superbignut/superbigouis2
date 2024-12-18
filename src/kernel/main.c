#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"

int magic = OS_MAGIC;

char msg[] = "Running C Code...";

char buf[1024];

void kernel_init(){
    char  * video = (char *) 0xb8000;
    
    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }  

    console_init();



}