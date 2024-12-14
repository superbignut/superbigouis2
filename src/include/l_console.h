#ifndef LCONSOLE_H_
#define LCONSOLE_H_
#include "l_types.h"
#include "l_io.h"

void console_init();

void console_clear();

void console_write(char *buf, uint32_t count);



#endif
