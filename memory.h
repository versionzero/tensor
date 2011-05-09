
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>

#define CASTED_MALLOC(x)        (x*) malloc_or_die(sizeof(x));
#define CASTED_MALLOC_N(x,n) (x*) malloc_or_die(n*sizeof(x));

#defome GUARDED_MALLOC(x)       (x*)

/* Allocate memory or fail */
void *malloc_or_die(size_t size);

/* Free memory and set the pointer to null */
void safe_delete(void *p);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
