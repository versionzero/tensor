
#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdarg.h>

#define D_MESSAGE     1
#define D_INFORMATION 2
#define D_WARNING     4
#define D_ERROR       8
#define D_FATAL      16

void message(char const *format, ...);
void information(char const *format, ...);
void warning(char const *format, ...);
void error(unsigned int type, char const *format, ...);
void die(char const *format, ...);

#endif

