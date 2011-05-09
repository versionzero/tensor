
#include "error.h"
#include "memory.h"
#include <stdlib.h>

void*
malloc_or_die(size_t size)
{
  void *p;
  
  if (NULL == (p = malloc(size))) {
    die("Failed to allocate memory %d.\n", size);
  }
  
  return p;
}

void
safe_delete(void *p)
{
  if (NULL != p ){
    free(p);
  }
  p = NULL;
}

