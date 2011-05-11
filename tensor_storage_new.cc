
#include "error.h"
#include "memory.h"
#include "storage.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void*
tensor_storage_coordinate_strategy_new(uint nnz)
{
  storage_coordinate_t *storage;
  
  information("tensor_storage_coordinate_strategy_new(nnz=%d)\n", nnz);
  
  storage = MALLOC(storage_coordinate_t);
  
  information("tensor_storage_coordinate_strategy_new: storage=0x%x\n", storage);
  
  storage->nnz    = nnz;
  storage->values = MALLOC_N(double, nnz);
  storage->I      = MALLOC_N(uint, nnz);
  storage->J      = MALLOC_N(uint, nnz);
  storage->K      = MALLOC_N(uint, nnz);
  
  return storage;
}

void*
tensor_storage_ekmr_strategy_new(uint nnz)
{
  storage_ekmr_t *storage;
  
  information("tensor_storage_ekmr_strategy_new(nnz=%d)\n", nnz);
  
#if 0
  storage         = MALLOC(storage_ekmr_t);
  storage->values = MALLOC_N(double, nnz);
  storage->I      = MALLOC_N(uint, nnz);
  storage->J      = MALLOC_N(uint, nnz);  
#endif

  return storage;
}
