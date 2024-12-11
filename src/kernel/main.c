#include "os.h"
#include "types.h"
#include "io.h"
int magic = OS_MAGIC;

char msg[] = "Running C Code...";

char buf[1024];

#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5

#define CRT_CUSOR_HIGH 0xe
#define CRT_CUSOR_LOW 0xf

void kernel_init(){
    char  * video = (char *) 0xb8000;
    for(int i=0; i< sizeof(msg); ++i){
        video[(i+320) * 2] = msg[i];   
    }  

    uint8_t data = input_byte(CRT_DATA_REG);

}