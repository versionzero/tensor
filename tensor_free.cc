
#include "error.h"
#include "memory.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <assert.h>

void
tensor_storage_free(tensor_storage_base_t *storage)
{
  superfluous("tensor_storage_free((tensor_storage_base_t*)0x%x)\n", storage);
  
  safe_free(storage->callbacks);
}

void
tensor_storage_free(tensor_storage_coordinate_t *storage)
{
  superfluous("tensor_storage_free((tensor_storage_coordinate_t*)0x%x)\n", storage);
  
  safe_free(storage->tuples);
}

void
tensor_storage_free(tensor_storage_compressed_t *storage)
{
  superfluous("tensor_storage_free((tensor_storage_compressed_t*)0x%x)\n", storage);
  
  safe_free(storage->RO);
  safe_free(storage->CO);
  safe_free(storage->TO);
  safe_free(storage->KO);
}

void
tensor_storage_free(tensor_storage_extended_t *storage)
{
  superfluous("tensor_storage_free((tensor_storage_extended_t*)0x%x)\n", storage);
  
  safe_free(storage->RO);
  safe_free(storage->CK);
}

void
tensor_storage_free(tensor_t *tensor)
{
  superfluous("tensor_storage_free(0x%x)\n", tensor);
  
  if (!tensor->storage) {
    return;
  }
  
  tensor_storage_free(STORAGE_BASE(tensor));
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    tensor_storage_free(STORAGE_COORIDINATE(tensor));
    break;
  case strategy::compressed:
  case strategy::slice:
    tensor_storage_free(STORAGE_COMPRESSED(tensor));
    break;
  case strategy::ekmr:
  case strategy::zzekmr:
    tensor_storage_free(STORAGE_EXTENDED(tensor));
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
  superfluous("tensor_free(0x%x)\n", tensor);
  
  if (!tensor) {
    return;
  }
  
  safe_free(tensor->values);
  tensor_storage_free(tensor);
  safe_free(tensor);
}

