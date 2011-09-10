
#include "strings.h"
#include <ctype.h>
#include <string.h>

char const*
bool_to_string(bool b)
{
  static char const *true_string  = "true";
  static char const *false_string = "false";
  return (b ? true_string : false_string);
}

char*
basename_inplace(char *name)
{
  char *base, *dot;

  base = strrchr(name, '/');
  if (NULL != base && '/' == *base) {
    base++;
  } else {
    base = name;
  }
  if (NULL != (dot = strrchr(name, '.'))) {
    *dot = '\0';
  }
  
  return base;
}

char*
lc(char *s)
{
  char *p;
  
  for (p = s; *p; ++p) {
    *p = tolower(*p);
  }
  
  return s;
}

char*
uc(char *s)
{
  char *p;
  
  for (p = s; *p; ++p) {
    *p = toupper(*p);
  }
  
  return s;
}
