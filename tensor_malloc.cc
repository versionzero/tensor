
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_malloc(uint l, uint m, uint n, uint nnz, strategy::type_t strategy, orientation::type_t orientation, bool naive)
{
  tensor_t *tensor;
  
  debug("tensor_malloc(l=%d, m=%d, n=%d, nnz=%d, strategy='%s', orientation='%s', naive='%s')\n",
	l, m, n, nnz,
	strategy_to_string(strategy),
	orientation_to_string(orientation),
	bool_to_string(naive));
  
  tensor              = MALLOC(tensor_t);
  tensor->l           = l;
  tensor->m           = m;
  tensor->n           = n;
  tensor->nnz         = nnz;
  tensor->strategy    = strategy;
  tensor->orientation = orientation;
  tensor->values      = NULL;
  tensor->storage     = NULL;
  
  if (nnz > 0) {
    tensor->values  = MALLOC_N(double, nnz);
    tensor->storage = storage_malloc(tensor, naive);
  }
  
  debug("tensor_malloc: tensor->values=0x%x\n", tensor->values);
  debug("tensor_malloc: tensor->storage=0x%x\n", tensor->storage);
  debug("tensor_malloc: tensor=0x%x\n", tensor);

  return tensor;
}

tensor_t*
tensor_malloc_from_template(tensor_t const *tensor)
{
  debug("tensor_malloc_from_template(tensor=0x%x)\n", tensor);
  
  return tensor_malloc(tensor->l, tensor->m, tensor->n, tensor->nnz, tensor->strategy, tensor->orientation);
}
