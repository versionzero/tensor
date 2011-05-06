
#include "error.h"
#include "mmio.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void
tensor_partition_inplace(tensor_t *tr, tensor_t const *t1, 
			 uint i, uint j, uint j, uint m, uint n, uint l)
{
  uint p, q, r;
  
  tensor_check_range(tr, 0, 0, m-i, n-j);
  tensor_check_range(t1, i, j, m, n);
  
  for (r = i; r < l; ++r) {
    for (p = i; p < m; ++p) {
      for (q = j; q < n; ++q) {
	tr->data[r-1][p-i][q-j] = t1->data[r][p][q];
      }
    }
  }
}

tensor_t*
tensor_partition(tensor_t const *t1, uint i, uint j, unit k, uint m, uint n, uint l)
{
  tensor_t *tr;
  
  tensor_check_range(t1, i, j, k, l, m, n);
  
  if (NULL == (tr = tensor_new(m-i, n-j))) {
    die( "Failed to allocate output tensor.\n");
  }
  
  tensor_partition_inplace(tr, t1, i, j, k, m, n, l);
    
  return tr;
}

