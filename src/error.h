
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
    message     = 1,        /* stdout: general, undecorated user messages */
    progress    = (1 << 1), /* stderr: used to tag 'busy' or processing messages */
    information = (1 << 2), /* stderr: misc. information that may be of use to a user */
    trace       = (1 << 3), /* stderr: debug information that may be helpful outside of debugging */
    debug       = (1 << 4), /* stderr: debug information; likely only useful in case of an error */
    warning     = (1 << 5), /* stderr: a minor, recoverable error occured */
    error       = (1 << 6), /* stderr: a major error occured */
    fatal       = (1 << 7)  /* stderr: an unrecoverable error occured */
  } type_t;
}

#if !defined (NODEBUG)
#define DEBUG(...) message(__VA_ARGS__)
#else
#define DEBUG(...)
#endif 

void message(char const *format, ...);
void progress(char const *format, ...);
void information(char const *format, ...);
void trace(char const *format, ...);
void debug(char const *format, ...);
void debug(verbosity::type_t verbosity, char const *format, ...);
void superfluous(char const *format, ...);
void warning(char const *format, ...);
void die(char const *format, ...);
void verror(uint levels, char const *format, va_list args);

#endif

