
#include "utility.h"

bool
is_power_of_two(uint n)
{
  return (0 != n && (n & (n-1)));
}
