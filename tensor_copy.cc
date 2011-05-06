
#include "error.h"
#include "mmio.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void
tensor_copy_shallow_inplace(tensor_t *tr, tensor_t const *t1)
{
  information("Tensor Copy (inplace, shallow)\n");
  
  tr->owner = viewer;
  tr->data  = t1->data;
}

tensor_t*
tensor_copy_shallow(tensor_t const *t1)
{
  tensor_t *tr;
  
  information("Tensor Copy (shallow)\n");  
  tr = tensor_new_or_die(t1->l, t1->m, t1->n, viewer);
  tensor_copy_shallow_inplace(tr, t1);

  return tr;
}

void
tensor_copy_inplace_with_offset(tensor_t *tr, uint oi, uint oj, tensor_t const *t1)
{
  uint i, j;
  
  information("Tensor Copy (inplace, with offset)");  
  tensor_check_range(tr, oi, oj, t1->m+oi, t1->n+oj);
  
  for (i = 0; i < t1->m; ++i) {
    for (j = 0; j < t1->n; ++j) {
      // tr->data[oi+i][oj+j] = t1->data[i][j];
    }
  }
}

void
tensor_copy_inplace(tensor_t *tr, tensor_t const *t1)
{
  uint i, j;
  
  information("Tensor Copy (inplace)\n");  
  tensor_compatible(tr, t1);
  
  for (i = 0; i < t1->m; ++i) {
    for (j = 0; j < t1->n; ++j) {
      // tr->data[i][j] = t1->data[i][j];
    }
  }
}

tensor_t*
tensor_copy(tensor_t const *t1)
{
  tensor_t *tr;
  
  information("Tensor Copy\n");  
  tr = tensor_new_or_die(t1->l, t1->m, t1->n);
  tensor_copy_inplace(tr, t1);
  
  return tr;
}
