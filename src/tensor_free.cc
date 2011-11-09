
#include "error.h"
#include "memory.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <assert.h>

void
tensor_free(tensor_t *tensor)
{
  superfluous("tensor_free(0x%x)\n", tensor);
  
  if (!tensor) {
    return;
  }
  
  if (ownership::creator == tensor->owner) {
    safe_free(tensor->values);
  }
  
  safe_free(tensor);
}

