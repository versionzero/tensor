
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

uint
tensor_index(tensor_t const *tensor, uint i, uint j, uint k) {
  return (i*tensor->n*tensor->m) + (j*tensor->m) + k;
}

tensor_t*
tensor_malloc(uint l, uint m, uint n, ownership::type_t owner)
{
  tensor_t *tensor;
  
  superfluous("tensor_malloc(l=%d, m=%d, n=%d)\n", l, m, n);
  
  tensor              = MALLOC(tensor_t);
  tensor->l           = l;
  tensor->m           = m;
  tensor->n           = n;
  tensor->strategy    = strategy::array;
  tensor->orientation = orientation::unknown;
  tensor->owner       = owner;
  tensor->values      = NULL;
  
  if (ownership::viewer == owner) {
    return tensor;
  }
  
  tensor->values      = MALLOC_N(double, l*m*n);
    
  superfluous("tensor_malloc: tensor->values=0x%x\n", tensor->values);
  superfluous("tensor_malloc: tensor=0x%x\n", tensor);

  return tensor;
}

tensor_t*
tensor_malloc(uint l, uint m, uint n, strategy::type_t strategy, orientation::type_t orientation, ownership::type_t owner)
{
  tensor_t *tensor;
  
  superfluous("tensor_malloc(l=%d, m=%d, n=%d, strategy='%s', orientation='%s')\n",
	l, m, n, strategy_to_string(strategy), orientation_to_string(orientation));
  
  tensor              = MALLOC(tensor_t);
  tensor->l           = l;
  tensor->m           = m;
  tensor->n           = n;
  tensor->strategy    = strategy;
  tensor->orientation = orientation;
  tensor->owner       = owner;
  tensor->values      = NULL;
  
  if (ownership::viewer == owner) {
    return tensor;
  }
  
  tensor->values      = MALLOC_N(double, l*m*n);
  
  superfluous("tensor_malloc: tensor->values=0x%x\n", tensor->values);
  superfluous("tensor_malloc: tensor=0x%x\n", tensor);

  return tensor;
}

tensor_t*
tensor_malloc_from_template(tensor_t const *tensor)
{
  superfluous("tensor_malloc_from_template(tensor=0x%x)\n", tensor);
  
  return tensor_malloc(tensor->l, tensor->m, tensor->n, tensor->strategy, tensor->orientation, tensor->owner);
}
