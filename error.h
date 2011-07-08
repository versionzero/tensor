
#ifndef _ERROR_H_
#define _ERROR_H_

#include "types.h"
#include <stdarg.h>

namespace verbosity {
  typedef enum {
    none    = 0,
    low     = 10,
    medium  = 50,
    high    = 100,
    max     = high
  } type_t;
}

namespace level {
  typedef enum {
    message     = 1,
    information = (1 << 1),
    debug       = (1 << 2),
    warning     = (1 << 3),
    error       = (1 << 4),
    fatal       = (1 << 5)
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
void debug(verbosity::type_t verbosity, char const *format, ...);
void superfluous(char const *format, ...);
void warning(char const *format, ...);
void die(char const *format, ...);

#endif

