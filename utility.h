
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <time.h>

/* True and False for our purposes */
#define FALSE 0
#define TRUE  1

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
 
#endif /* _UTILITY_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
