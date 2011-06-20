
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern char *tool_name;
extern bool verbose;

void
verror(uint type, char const *format, va_list args)
{
  bool show;
  
  show = verbose || !(type & (D_INFORMATION | D_DEBUG));
  if (show) {
    if (!(type & D_MESSAGE)) {
      fprintf(stderr, "%s: ", tool_name);
    }
    if (type & D_INFORMATION) {
      fprintf(stderr, "INFORMATION: ");
    } else if (type & D_DEBUG) {
      fprintf(stderr, "DEBUG: ");
    } else if (type & D_WARNING) {
      fprintf(stderr, "WARNING: ");
    } else if (type & D_ERROR) {
      fprintf(stderr, "ERROR: ");
    }
    vfprintf(stderr, format, args);
  }
  if (type & D_FATAL) {
    exit(type);
  }
}

void
message(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(D_MESSAGE, format, args);
  va_end(args);
}

void
information(char const *format, ...)
{
  va_list args;

  va_start(args, format);
  verror(D_INFORMATION, format, args);
  va_end(args);
}

void
debug(char const *format, ...)
{
  va_list args;

  va_start(args, format);
  verror(D_DEBUG, format, args);
  va_end(args);
}

void
warning(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(D_WARNING, format, args);
  va_end(args);
}

void
error(uint type, char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(type, format, args);
  va_end(args);
}

void
die(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(D_ERROR|D_FATAL, format, args);
  va_end(args);
}

