
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "types.h"
#include <time.h>

/* How close to zero is zero */
#define EPSILON 1e-10

/* Largest path for a file on disk (yes, I know this is evil...) */
#define PATH_MAX 256

/* Very simple calculation of elapsed time */
#define SECONDS_SINCE(t)  ((clock()-(t))/(double)CLOCKS_PER_SEC)

/* Count the number of elements in x[n] */
#define COUNT_OF(x) (sizeof(x)/sizeof(x[0]))

/* Determine if a number is a power of 2 */
bool is_power_of_two(uint n);

/* Determine if a number is as close to zero as we consider zero */
bool might_as_well_be_zero(double d);

/* Return a string version of a boolean type */
char const* bool_to_string(bool b);

/* Determine the appropriate name to base the output file names on.
   Note that this command is destructive, so the input buffer *will*
   be modified. */
char* mybasename(char *name);

/* ln(x), or log base 2 of x */
long double log2(long double x);
 
#endif /* _UTILITY_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
