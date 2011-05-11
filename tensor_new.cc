
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_new(uint l, uint m, uint n, uint nnz, storage_strategy_t strategy)
{
  uint     i, j, k;
  tensor_t *tensor;
  void     *p;
  
  information("tensor_new(l=%d, m=%d, n=%d, nnz=%d, strategy='%s')\n", 
	      l, m, n, nnz, tensor_storage_name(strategy));
  
  tensor           = MALLOC(tensor_t);
  tensor->l        = l;
  tensor->m        = m;
  tensor->n        = n;
  tensor->strategy = strategy;
  p                = NULL;
  
  information("tensor_new: tensor=0x%x\n", tensor);
  
  if (nnz > 0) {
    switch(strategy) {
    case coordinate:
      p = tensor_storage_coordinate_strategy_new(nnz);
      break;
    case ekmr:
      p = tensor_storage_ekmr_strategy_new(nnz);
      break;
    }    
  }
  
  tensor->storage = p;
  
  information("tensor_new: tensor->storage=0x%x\n", tensor->storage);
  
  return tensor;
}
