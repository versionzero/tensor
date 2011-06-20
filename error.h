
#ifndef _ERROR_H_
#define _ERROR_H_

#include "types.h"
#include <stdarg.h>

#define D_MESSAGE     1
#define D_INFORMATION 2
#define D_DEBUG       4
#define D_WARNING     8
#define D_ERROR      16
#define D_FATAL      32

#if !defined (NODEBUG)
#define DEBUG(...) message(__VA_ARGS__)
#else
#define DEBUG(...)
#endif 

void message(char const *format, ...);
void information(char const *format, ...);
void debug(char const *format, ...);
void warning(char const *format, ...);
void error(uint type, char const *format, ...);
void die(char const *format, ...);

#endif

