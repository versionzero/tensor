
#include "error.h"
#include "matrix.h"
#include "memory.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

matrix_t*
matrix_malloc(uint m, uint n, ownership::type_t owner)
{
  uint     i;
  matrix_t *mr;
  
  mr        = MALLOC(matrix_t);
  mr->m     = m;
  mr->n     = n;
  mr->data  = NULL;
  mr->owner = owner;
  
  if (ownership::viewer == owner) {
    return mr;
  }
  
  mr->data = MALLOC_N(double*, m);
  for (i = 0; i < m; ++i) {
    mr->data[i] = MALLOC_N(double, n);
  }
  
  return mr;
}
