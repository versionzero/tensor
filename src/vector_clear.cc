
#include "error.h"
#include "vector.h"

void
vector_clear(vector_t *v)
{
  uint i;
  
  debug("vector_clear(vector=0x%x)\n", v);
  
  for (i = 0; i < v->n; ++i) {
    v->data[i] = 0.0;
  }
}

