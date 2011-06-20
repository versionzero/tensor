
#include "error.h"
#include "tensor.h"

void
storage_clear_coordinate(tensor_t *tensor)
{
  uint i;
  storage_coordinate_t *storage;
  coordinate_tuple_t   *tuples;
  
  debug("storage_clear_coordinate(0x%x)\n", tensor);
  
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  
  for (i = 0; i < tensor->nnz; ++i) {
    tuples[i].i     = 0;
    tuples[i].j     = 0;
    tuples[i].k     = 0;
    tuples[i].index = 0;
  }
}

void
storage_clear_compressed(tensor_t *tensor)
{
  uint i;
  storage_compressed_t *storage;
  
  debug("storage_clear_compressed(0x%x)\n", tensor);
  
  storage = STORAGE_COMPRESSED(tensor);
  
  for (i = 0; i < storage->size; ++i) {
    storage->RO[i] = 0;
  }
  
  for (i = 0; i < tensor->nnz; ++i) {
    storage->CO[i] = 0;
    storage->KO[i] = 0;
  }
}

void
storage_clear_ekmr(tensor_t *tensor)
{
  uint i;
  storage_ekmr_t *storage;
  
  debug("storage_clear_ekmr(0x%x)\n", tensor);
  
  storage = STORAGE_EKMR(tensor);
  
  for (i = 0; i < storage->size; ++i) {
    storage->RO[i] = 0;
  }
  
  for (i = 0; i < tensor->nnz; ++i) {
    storage->CK[i] = 0;
  }
}

void
tensor_clear(tensor_t *tensor)
{
  uint i;
  
  debug("tensor_clear(0x%x)\n", tensor);
  tensor_validate(tensor);
  
  for (i = 0; i < tensor->nnz; ++i) {
    tensor->values[i] = 0.0;
  }
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    storage_clear_coordinate(tensor);
    break;
  case strategy::compressed:
    storage_clear_compressed(tensor);
    break;
  case strategy::ekmr:
    storage_clear_ekmr(tensor);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", tensor->strategy);
  }
}

