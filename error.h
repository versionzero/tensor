
#ifndef _ERROR_H_
#define _ERROR_H_

#include "types.h"
#include <stdarg.h>

namespace level {
  typedef enum {
    message     = 1,
    information = 2,
    debug       = 4,
    warning     = 8,
    error       = 16,
    fatal       = 32
  } type_t;
}

#if !defined (NODEBUG)
#define DEBUG(...) message(__VA_ARGS__)
#else
#define DEBUG(...)
#endif 

void message(char const *format, ...);
void information(char const *format, ...);
void debug(char const *format, ...);
void warning(char const *format, ...);
void error(uint levels, char const *format, ...);
void die(char const *format, ...);

#endif

