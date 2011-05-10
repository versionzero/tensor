
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"
#include <stdio.h>

/* Convinience macros for allocating memory: with the firnge benefit
   of telling use the exact line number where the memory alloction
   failed. */
#define MALLOC(x)     ((x*)malloc_or_die(sizeof(x),__FILE__,__LINE__));
#define MALLOC_N(x,n) ((x*)malloc_or_die(n*sizeof(x),__FILE__,__LINE__));

/* Allocate memory or fail */
void *malloc_or_die(size_t size, char const* file, uint const line);

/* Free memory and set the pointer to null */
void safe_delete(void *p);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
