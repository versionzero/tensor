
#include "error.h"
#include "memory.h"
#include <stdlib.h>

void*
malloc_or_die(size_t size, char const* file, uint const line)
{
  void *p;
  
  superfluous("malloc_or_die(size=%d), %s:%d\n", size, file, line);
  
  if (NULL == (p = malloc(size))) {
    die("%s:%d: Failed to allocate memory block: %d.\n", file, line, size);
  }
  
  superfluous("malloc_or_die: p=0x%x\n", p);
  
  return p;
}

void*
calloc_or_die(size_t number, size_t size, char const* file, uint const line)
{
  void *p;
  
  superfluous("calloc_or_die(number=%d, size=%d), %s:%d\n", number, size, file, line);
  
  if (NULL == (p = calloc(number, size))) {
    die("%s:%d: Failed to allocate memory block: number=%d, size=%d.\n", file, line, number, size);
  }
  
  superfluous("calloc_or_die: p=0x%x\n", p);
  
  return p;
}

void
safe_free(void *p)
{
  superfluous("safe_free(0x%x)\n", p);
  
  if (!p){
    return;
  }
  
  free(p);
  p = NULL;
}

