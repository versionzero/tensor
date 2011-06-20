
#include "compatible.h"
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
tensor_convert_inplace(tensor_t *destination, tensor_t *source)
{
  debug("tensor_convert_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  compatible(destination, source);
  storage_convert_inplace(destination, source);
}

tensor_t*
tensor_convert(tensor_t *tensor, strategy::type_t strategy, orientation::type_t orientation)
{
  tensor_t *result;
  
  debug("tensor_convert(tensor=0x%x, strategy='%s', orientation='%s')\n", 
	tensor, strategy_to_string(strategy), orientation_to_string(orientation));
  
  result = tensor_malloc(tensor->l, tensor->m, tensor->n, tensor->nnz, strategy, orientation);
  tensor_convert_inplace(result, tensor);
  
  return result;
}
