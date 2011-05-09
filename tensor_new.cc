
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
  tensor_t *tr;
  void     *p;
  
  if (NULL == (tr = (tensor_t*) malloc(sizeof(tensor_t)))) {
    die("Failed to allocate tensor.\n");
  }
  
  tr->strategy = strategy;
  p            = NULL;
  
  if (0 == nnz ) {
    switch(strategy) {
    case coordinate:
      p = tensor_storage_coordinate_strategy_new(nnz);
      break;
    case ekmr:
      p = tensor_storage_ekmr_strategy_new(nnz);
      break;
    }    
  }

  tr->storage = p;

  return tr;
}
