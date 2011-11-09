
#include "error.h"
#include "tensor.h"

void
tensor_clear(tensor_t *tensor)
{
  uint i, n;
  
  debug("tensor_clear(0x%x)\n", tensor);
  tensor_validate(tensor);
  
  n = tensor->l*tensor->m*tensor->n;
  
  for (i = 0; i < n; ++i) {
    tensor->values[i] = 0.0;
  }
}

