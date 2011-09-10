
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

long double
log2(long double x)
{
  return log(x)/log(2);
}
