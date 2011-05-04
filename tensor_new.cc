
#include "error.h"
#include "tensor.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_new(uint l, uint m, uint n, uint owner)
{
  uint     i, j, k;
  tensor_t *result;
  double   *p;
  
  if (NULL == (result = (tensor_t*) malloc(sizeof(tensor_t)))) {
    die("Failed to allocate tensor.\n");
  }

  result->l     = l;
  result->m     = m;
  result->n     = n;
  result->data  = NULL;
  result->owner = owner;
  
  if (VIEWER == owner) {
    return result;
  }

  if (NULL == (p = (double*) malloc(l*m*n*sizeof(double)))) {
    die("Failed to allocate tensor storage.\n");
  }

  result->storage = p;

  if (NULL == (result->data = (double***) malloc(l*sizeof(double)))) {
    die("Failed to allocate tensor slice data layout.\n");
  }
  for (i = 0; i < l; ++i) {
    if (NULL == (result->data[i] = (double**) malloc(m*sizeof(double)))) {
      die("Failed to allocate row data layout.\n");
    }
    for (j = 0; j < m; ++j) {
      result->data[i][j] = p;
      p += n;
    }    
  }

  return result;
}

