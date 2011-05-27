
#include "error.h"
#include "memory.h"
#include <stdlib.h>

void*
malloc_or_die(size_t size, char const* file, uint const line)
{
  void *p;
  
  debug("malloc_or_die(size=%d), %s:%d\n", size, file, line);
  
  if (NULL == (p = malloc(size))) {
    die("%s:%d: Failed to allocate memory block: %d.\n", file, line, size);
  }
  
  debug("malloc_or_die: p=0x%x\n", p);
  
  return p;
}

void
safe_free(void *p)
{
  debug("safe_free(0x%x)\n", p);
  
  if (!p){
    return;
  }
  free(p);
  p = NULL;
}

