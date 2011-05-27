
#include "error.h"
#include "memory.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <assert.h>

void
storage_free(storage_base_t *storage)
{
  debug("storage_free((storage_base_t*)0x%x)\n", storage);
  
  safe_free(storage->callbacks);
}

void
storage_free(storage_coordinate_t *storage)
{
  debug("storage_free((storage_coordinate_t*)0x%x)\n", storage);
  
  safe_free(storage->tuples);
}

void
storage_free(storage_compressed_t *storage)
{
  debug("storage_free((storage_compressed_t*)0x%x)\n", storage);
  
  safe_free(storage->RO);
  safe_free(storage->CO);
  safe_free(storage->KO);
}

void
storage_free(storage_ekmr_t *storage)
{
  debug("storage_free((storage_ekmr_t*)0x%x)\n", storage);
  
  safe_free(storage->R);
  safe_free(storage->CK);
}

void
storage_free(tensor_t *tensor)
{
  debug("storage_free(0x%x)\n", tensor);
  
  if (!tensor->storage) {
    return;
  }
  
  storage_free(STORAGE_BASE(tensor));
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    storage_free(STORAGE_COORIDINATE(tensor));
    break;
  case strategy::compressed:
    storage_free(STORAGE_COMPRESSED(tensor));
    break;
  case strategy::ekmr:
    storage_free(STORAGE_EKMR(tensor));
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", 
	strategy_to_string(tensor->strategy));
  }
  
  safe_free(tensor->storage);
}

void
tensor_free(tensor_t *tensor)
{
  debug("tensor_free(0x%x)\n", tensor);
  
  if (!tensor) {
    return;
  }
  
  safe_free(tensor->values);
  storage_free(tensor);
  safe_free(tensor);
}

