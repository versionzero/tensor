
#include "error.h"
#include "tensor.h"

void
tensor_transfer_ownership(tensor_t *destination, tensor_t *source)
{
  superfluous("tensor_transfer_ownership(destination=0x%x, source=0x%x)\n", destination, source);
  
  if (ownership::creator == destination->owner) {
    die("tensor_transfer_ownership: destination tensor already owns data\n");
  }
  
  tensor_copy_shallow(destination, source);
  
  destination->owner = ownership::creator;
  source->owner      = ownership::viewer;
}

