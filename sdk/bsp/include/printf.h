#ifndef _PRINTF_H_
#define _PRINTF_H_

#include <stdarg.h>
#include "typedef.h"

extern int putchar(int a);

extern int puts(const char *out);

extern void put_buf(const u8 *buf, int len);

extern int printf(const char *format, ...);

extern int sprintf(char *out, const char *format, ...);

extern int vprintf(const char *fmt, __builtin_va_list va);

extern int print(char **out, char *end, const char *format, va_list args);


#endif
