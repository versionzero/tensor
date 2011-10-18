
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void*
tensor_storage_malloc(tensor_t const *tensor)
{
  void *storage;
  
  superfluous("tensor_storage_malloc(tensor=0x%x [strategy='%s'])\n", 
	tensor, strategy_to_string(tensor->strategy));
  
  storage = NULL;
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    storage = tensor_storage_malloc_coordinate(tensor);
    break;
  case strategy::compressed:
    storage = tensor_storage_malloc_compressed(tensor);
    break;
  case strategy::slice:
    storage = tensor_storage_malloc_compressed_slice(tensor);
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
  
  superfluous("tensor_storage_malloc: storage=0x%x\n", storage);
  
  return storage;
}
