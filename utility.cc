
#include "string.h"
#include "utility.h"
#include <math.h>

bool
is_power_of_two(uint n)
{
  return (0 != n && 0 == (n & (n-1)));
}

bool
might_as_well_be_zero(double d)
{
  return (fabs(d) < EPSILON);
}

char const*
bool_to_string(bool b)
{
  static char const* true_string  = "true";
  static char const* false_string = "false";
  return (b ? true_string : false_string);
}

/* Determine the appropriate name to base the output file names on.
   Note that this command is destructive, so the input buffer *will*
   be modified. */
char*
mybasename(char *name)
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
