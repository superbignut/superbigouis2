#ifndef IO_H_
#define IO_H_

#include "types.h"

uint8_t read_byte_from_vga(uint16_t port);

uint16_t read_word_from_vga(uint16_t port); 

void write_byte_to_vga(uint16_t port, uint8_t value);

void write_word_to_vga(uint16_t port, uint16_t value);

#endif
