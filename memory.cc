
#include "error.h"
#include "memory.h"
#include <stdlib.h>

void*
malloc_or_die(size_t size, char const* file, uint const line)
{
  void *p;
  
  if (NULL == (p = malloc(size))) {
    die("%s:%d: Failed to allocate memory block: %d.\n", file, line, size);
  }
  
  return p;
}

void
safe_delete(void *p)
{
  if (NULL != p){
    free(p);
  }
  p = NULL;
}

