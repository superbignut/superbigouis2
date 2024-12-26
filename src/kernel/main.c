#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"
#include "l_stdargs.h"
#include "l_printk.h"

int magic = OS_MAGIC;

static void hello_c_code(){
    
    char msg[] = "Running C Code...";
    char *video = (char *)0xb8000;

    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }
}


void test(const char **fmt){
    (*fmt)++;
    return;
}
void kernel_init(){

    hello_c_code();

    console_init();

    char *a = "123\n123\n";

    int cnt = sizeof(a);

    console_write(a, string_len(a));

    while(cnt--){
        int b = printk("Hello operating system... %x %d\n", 0x123a, 123);
        //break;
    }

}