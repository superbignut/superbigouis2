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
#include "l_task.h"
#include "l_interrupt.h"

void func(int t){

}
void kernel_init(){

    console_init();
    
    gdt_init();
    // XBB;

    int cnt = 10;

    func(cnt);


    while(cnt--){
        int b = printk("Hello operating system...\n");
        //break;
    }

    // task_init();
    interrupt_init();
    

}