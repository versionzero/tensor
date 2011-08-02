
#include "error.h"
#include "latex.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern bool emit_latex;

void
latex(char const *format, ...)
{
  va_list args;
  
  if (emit_latex) {
    va_start(args, format);
    verror(level::information, format, args);
    va_end(args);
  }
}

