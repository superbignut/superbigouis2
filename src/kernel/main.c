#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
#include "l_console.h"
#include "l_stdargs.h"
#include "l_printk.h"
#include "l_assert.h"
#include "l_debug.h"

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

    char *a = "123\n123\n";

    int cnt = sizeof(a);

    console_write(a, string_len(a));

    assert(cnt == 4);

    XBB;
    DEBUGK("123\n");
    DEBUGK("No error occoured!\n");
    XBB;

    while(cnt--){
        int b = printk("Hello operating system... %x %d\n", 0x1213a, 123);
        //break;
    }

}