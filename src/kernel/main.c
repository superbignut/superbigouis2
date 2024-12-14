#include "l_os.h"
#include "l_types.h"
#include "l_io.h"
#include "l_string.h"
int magic = OS_MAGIC;

char msg[] = "Running C Code...";

char buf[1024];

void kernel_init(){
    char  * video = (char *) 0xb8000;
    
    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }  

    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_HIGH_INDEX); // 高地址索引

    write_byte_to_vga(CRT_DATA_REG_PORT, 0);
    
    write_byte_to_vga(CRT_ADDR_REG_PORT, CRT_CUSOR_LOW_INDEX); // 低地址索引
    
    write_byte_to_vga(CRT_DATA_REG_PORT, 100);

    uint8_t a = 1;

    int l = string_len(msg);

    memory_copy(buf, msg, sizeof(msg));
    msg[0] = 'Z';

    int mans = memory_compare(msg, buf, 10);
    
    int ans = string_compare(msg, buf);

    char * p = string_find_char_first(msg, 'C');



}