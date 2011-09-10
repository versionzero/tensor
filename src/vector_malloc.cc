
#include "error.h"
#include "vector.h"
#include "memory.h"

vector_t*
vector_malloc(uint n, ownership::type_t owner)
{
  vector_t *v;
  
  superfluous("vector_malloc(n=%d, owner='%s')\n", n, ownership_to_string(owner));
  
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
