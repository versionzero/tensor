
#include "error.h"
#include "vector.h"
#include "memory.h"

vector_t*
vector_malloc(uint n, ownership::type_t owner)
{
  vector_t *v;
  
  debug("vector_malloc(n=%d)\n", n);
  
  v        = MALLOC(vector_t);
  v->n     = n;
  v->data  = NULL;
  v->owner = owner;
  
  if (ownership::viewer == owner) {
    return v;
  }
  
  v->data  = MALLOC_N(double, n);  
  
  return v;
}
