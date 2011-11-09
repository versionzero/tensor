
#include "error.h"
#include "tensor.h"

void
tensor_validate(tensor_t const *tensor)
{
  if (!tensor) {
    die("Tensor structure has not been allocated.\n");
  }
  
  if (!tensor->values) {
    die("Tensor values have not been allocated.\n");
  }
}
