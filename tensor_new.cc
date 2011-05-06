
#include "error.h"
#include "tensor.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_new(uint l, uint m, uint n, ownership_t owner)
{
  uint     i, j, k;
  tensor_t *tr;
  double   *p;
  
  if (NULL == (tr = (tensor_t*) malloc(sizeof(tensor_t)))) {
    die("Failed to allocate tensor.\n");
  }

  tr->l     = l;
  tr->m     = m;
  tr->n     = n;
  tr->data  = NULL;
  tr->owner = owner;
  
  if (viewer == owner) {
    return tr;
  }

  if (NULL == (p = (double*) malloc(l*m*n*sizeof(double)))) {
    die("Failed to allocate tensor storage.\n");
  }

  tr->storage = p;

  if (NULL == (tr->data = (double***) malloc(l*sizeof(double)))) {
    die("Failed to allocate tensor slice data layout.\n");
  }
  for (i = 0; i < l; ++i) {
    if (NULL == (tr->data[i] = (double**) malloc(m*sizeof(double)))) {
      die("Failed to allocate row data layout.\n");
    }
    for (j = 0; j < m; ++j) {
      tr->data[i][j] = p;
      p += n;
    }    
  }

  return tr;
}

tensor_t*
tensor_new(uint l, uint m, uint n, ownership_t owner)
{
  tensor_t *tr;

  if (NULL == (tr = tensor_new(m, n))) {
    die("Failed to allocate tensor.\n");
  }
  
  return tr;
}
