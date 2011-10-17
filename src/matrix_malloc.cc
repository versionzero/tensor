
#include "error.h"
#include "matrix.h"
#include "memory.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

smatrix_t*
smatrix_malloc(uint m, uint n, uint nnz, ownership::type_t owner)
{
  smatrix_t *mr;
  
  superfluous("smatrix_malloc(m=%d, n=%d, nnz=%d, owner='%s')\n",
	      m, n, nnz, ownership_to_string(owner));
  
  mr         = MALLOC(smatrix_t);
  mr->m      = m;
  mr->n      = n;
  mr->rn     = 0;
  mr->RO     = NULL;
  mr->CO     = NULL;
  mr->values = NULL;
  mr->owner  = owner;
  mr->scheme = scheme::general;
  
  if (ownership::viewer == owner) {
    return mr;
  }
  
  if (nnz > 0) {
    mr->rn     = n > m ? n : m;
    mr->RO     = MALLOC_N(uint, mr->rn);
    mr->CO     = MALLOC_N(uint, nnz);
    mr->values = MALLOC_N(double, nnz);
  }
  
  return mr;
}
  

matrix_t*
matrix_malloc(uint m, uint n, ownership::type_t owner)
{
  uint     i;
  matrix_t *mr;
  
  superfluous("matrix_malloc(m=%d, n=%d, owner='%s')\n", m, n, ownership_to_string(owner));
  
  mr           = MALLOC(matrix_t);
  mr->m        = m;
  mr->n        = n;
  mr->data     = NULL;
  mr->owner    = owner;
  mr->scheme   = scheme::general;
  
  if (ownership::viewer == owner) {
    return mr;
  }
  
  mr->data = MALLOC_N(double*, m);
  for (i = 0; i < m; ++i) {
    mr->data[i] = MALLOC_N(double, n);
  }
  
  return mr;
}
