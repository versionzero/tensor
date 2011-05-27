
#include "error.h"
#include "tensor.h"

void
tensor_compatible(tensor_t const *lhs, tensor_t const *rhs)
{
  debug("tensor_compatible(lhs=0x%x, rhs=0x%x)\n", lhs, rhs);

  if (lhs->nnz != rhs->nnz) {
    die("Tensors do not have the same number non-zero entries.\n");
  }
  
  if (lhs->l != rhs->l || lhs->m != rhs->m || lhs->n != rhs->n) {
    die("Tensors do not have the same dimensions.\n");
  }
  
  if (rhs->strategy != strategy::coordinate) {
    die("Conversion from '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
}

