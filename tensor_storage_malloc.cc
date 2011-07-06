
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

tensor_storage_coordinate_t*
tensor_storage_malloc_coordinate(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_coordinate_t *storage;
  
  debug("tensor_storage_malloc_coordinate(tensor=0x%x, nnz=%d)\n", tensor, tensor->nnz);
  
  storage         = MALLOC(tensor_storage_coordinate_t);
  storage->tuples = MALLOC_N(coordinate_tuple_t, tensor->nnz);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = NULL;
  
  debug("tensor_storage_malloc_coordinate: storage->tuples=0x%x\n", storage->tuples);
  debug("tensor_storage_malloc_coordinate: storage=0x%x\n", storage);
  
  return storage;
}

void*
tensor_storage_malloc(tensor_t const *tensor)
{
  void *storage;
  
  debug("tensor_storage_malloc(tensor=0x%x [strategy='%s'])\n", 
	tensor, strategy_to_string(tensor->strategy));
  
  storage = NULL;
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    storage = tensor_storage_malloc_coordinate(tensor);
    break;
  case strategy::compressed:
    storage = tensor_storage_malloc_compressed(tensor);
    break;
  case strategy::ekmr:
    storage = tensor_storage_malloc_ekmr(tensor);
    break;
  case strategy::zzekmr:
    storage = tensor_storage_malloc_zzekmr(tensor);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", tensor->strategy);
  }
  
  debug("tensor_storage_malloc: storage=0x%x\n", storage);
  
  return storage;
}
