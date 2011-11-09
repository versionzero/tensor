
#include "error.h"
#include "tensor.h"

void
tensor_copy_shallow(tensor_t *destination, tensor_t *source)
{
  debug("tensor_copy_shallow(destination=0x%x, source=0x%x)\n", destination, source);
  
  destination->owner   = ownership::viewer;
  destination->values  = source->values;
}

tensor_t*
tensor_copy_shallow(tensor_t *source)
{
  tensor_t *destination;
  
  debug("tensor_copy_shallow(source=0x%x)\n", source);
  
  destination = tensor_malloc(source->l, source->m, source->n, source->strategy, source->orientation, source->owner);
  tensor_copy_shallow(destination, source);
  
  return destination;
}
