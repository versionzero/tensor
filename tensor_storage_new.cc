
#include "error.h"
#include "memory.h"
#include "storage.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void*
tensor_storage_coordinate_strategy_new(uint nnz)
{
  coordinate_storage_t *storage;
  
  storage         = MALLOC(coordinate_storage_t);
  storage->values = MALLOC_N(double, nnz);
  storage->I      = MALLOC_N(uint, nnz);
  storage->J      = MALLOC_N(uint, nnz);
  storage->K      = MALLOC_N(uint, nnz);
  
  return storage;
}

void*
tensor_storage_ekmr_strategy_new(uint nnz)
{
  ekmr_storage_t *storage;
  
#if 0
  storage         = MALLOC(ekmr_storage_t);
  storage->values = MALLOC_N(double, nnz);
  storage->I      = MALLOC_N(uint, nnz);
  storage->J      = MALLOC_N(uint, nnz);  
#endif

  return storage;
}
