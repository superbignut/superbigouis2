#ifndef LSTDIO_H_
#define LSTDIO_H_


#include "l_stdargs.h"
#include "l_string.h"
#include "l_types.h"

int vsprintf(char *buf, const char *fmt, va_list args);

int sprintf(char *buf, const char *fmt, ...);

#endif