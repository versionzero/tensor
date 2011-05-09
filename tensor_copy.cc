
#include "error.h"
#include "mmio.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void
tensor_copy_inplace(tensor_t *tr, tensor_t const *t1)
{
  uint i, j, k;
  
  information("Tensor Copy (inplace)\n");  
  tensor_compatible(tr, t1);
  
  for (k = 0; k < t1->l; ++k) {
    for (i = 0; i < t1->m; ++i) {
      for (j = 0; j < t1->n; ++j) {
	// tr->data[k][i][j] = t1->data[k][i][j];
      }
    }
  }
}

tensor_t*
tensor_copy(tensor_t const *t1)
{
  tensor_t *tr;
  
  information("Tensor Copy\n");  
  tr = tensor_new(t1->l, t1->m, t1->n);
  tensor_copy_inplace(tr, t1);
  
  return tr;
}
