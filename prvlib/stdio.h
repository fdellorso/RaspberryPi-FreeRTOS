#ifndef _STDIO_H
#define _STDIO_H

#include <uspi/types.h>
#include <uspi/stdarg.h>

int vsnprintf (char *s, size_t n, const char *format, va_list ap);
int sprintf(char *s, const char *format, ...);

#endif