
#include "error.h"
#include "tensor.h"
#include "storage.h"

#include <stdio.h>
#include <stdlib.h>

void*
tensor_storage_coordinate_strategy_new(uint nnz)
{
  coordinate_storage_t *storage;
  
  storage         = CASTED_MALLOC(coordinate_storage_t);
  storage->values = CASTED_MALLOC_N(double, nnz);
  storage->I      = CASTED_MALLOC_N(uint, nnz);
  storage->J      = CASTED_MALLOC_N(uint, nnz);
  storage->K      = CASTED_MALLOC_N(uint, nnz);
  
  return storage;
}

void*
tensor_storage_ekmr_strategy_new(uing nnz)
{
  ekmr_storage_t *storage;
  
#if 0
  storage         = CASTED_MALLOC(ekmr_storage_t);
  storage->values = CASTED_MALLOC_N(double, nnz);
  storage->I      = CASTED_MALLOC_N(uint, nnz);
  storage->J      = CASTED_MALLOC_N(uint, nnz);  
#endif

  return storage;
}

tensor_t*
tensor_new(uint l, uint m, uint n, uint nnz, storage_strategy_t s, ownership_t o)
{
  uint     i, j, k;
  tensor_t *tr;
  void     *p;
  
  tr           = (tensor_t*) malloc_or_die(sizeof(tensor_t));
  tr->l        = l;
  tr->m        = m;
  tr->n        = n;
  tr->owner    = o;
  tr->strategy = s;
  tr->storage  = NULL;  
  
  if (viewer == owner) {
    return tr;
  }

  switch(s) {
  case coordinate:
    p = tensor_new_coordinate_storage(nnz);
    break;
  case ekmr:
    p = tensor_new_ekmr_storage(nnz);
    break;
  }

  tr->storage = p;

  return tr;
}

tensor_t*
tensor_new(uint l, uint m, uint n, ownership_t o)
{
  tensor_t *tr;

  tr           = (tensor_t*) malloc_or_die(sizeof(tensor_t));
  tr->l        = l;
  tr->m        = m;
  tr->n        = n;
  tr->owner    = o;
  tr->strategy = none;
  tr->storage  = NULL;
  
  return tr;
}
