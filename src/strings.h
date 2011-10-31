
#ifndef _STRINGS_H_
#define _STRINGS_H_

#include "types.h"

/* Return a string version of a boolean type */
char const* bool_to_string(bool b);

/* Determine the appropriate name to base the output file names on.
   Note that this command is destructive, so the input buffer *will*
   be modified. */
char* destructive_basename(char *name);

/* Upper/Lower-case a string (inplace) */
char* lc(char *s);
char* uc(char *s);
 
#endif /* _STRINGS_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
