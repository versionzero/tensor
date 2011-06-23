
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

storage_coordinate_t*
storage_malloc_coordinate(tensor_t const *tensor)
{
  storage_base_t       *base;
  storage_coordinate_t *storage;
  
  debug("storage_malloc_coordinate(tensor=0x%x, nnz=%d)\n", tensor, tensor->nnz);
  
  storage         = MALLOC(storage_coordinate_t);
  storage->tuples = MALLOC_N(coordinate_tuple_t, tensor->nnz);
  base            = (storage_base_t*) storage;
  base->callbacks = NULL;
  
  debug("storage_malloc_coordinate: storage->tuples=0x%x\n", storage->tuples);
  debug("storage_malloc_coordinate: storage=0x%x\n", storage);
  
  return storage;
}

void*
storage_malloc(tensor_t const *tensor)
{
  void *storage;
  
  debug("storage_malloc(tensor=0x%x [strategy='%s'])\n", 
	tensor, strategy_to_string(tensor->strategy));
  
  storage = NULL;
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    storage = storage_malloc_coordinate(tensor);
    break;
  case strategy::compressed:
    storage = storage_malloc_compressed(tensor);
    break;
  case strategy::ekmr:
    storage = storage_malloc_ekmr(tensor);
    break;
  case strategy::pkmr:
    storage = storage_malloc_pkmr(tensor);
    break;
  case strategy::zzpkmr:
    storage = storage_malloc_zzpkmr(tensor);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", tensor->strategy);
  }
  
  debug("storage_malloc: storage=0x%x\n", storage);
  
  return storage;
}
