#include <stdio.h>
#include <types.h>



typedef struct des{
    uint32_t age;//: 24;
    uint8_t a;
    uint32_t c;
    uint8_t b;
    uint16_t name;
} _packed des;

int main(){


    des a;
    printf(" size of %d", sizeof(a));

    return 0;
}