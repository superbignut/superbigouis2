#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"
#include "l_stdargs.h"
#include "l_printk.h"
#include "l_assert.h"
#include "l_debug.h"
#include "l_gdt.h"

int magic = OS_MAGIC;

static void hello_c_code(){
    
    char msg[] = "Running C Code...";
    char *video = (char *)0xb8000;

    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }
}


void kernel_init(){

    hello_c_code();

    console_init();
    
    gdt_init();
    
    int cnt = 1;

    while(cnt--){
        int b = printk("Hello operating system...\n");
        //break;
    }

}