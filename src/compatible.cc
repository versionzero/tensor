
#include "compatible.h"
#include "error.h"
#include "information.h"

void
compatible(vector_t const *lhs, tensor_t const *rhs)
{
  bool compatible, supported;
  
  debug("compatible(vector=0x%x, tensor=0x%x)\n", lhs, rhs);
  
  switch (rhs->strategy) {
  case strategy::array:
  case strategy::compressed:
  case strategy::slice:
  case strategy::ekmr:
  case strategy::zzekmr:
    supported = true;
    break;
  default:
    supported = false;
    break;
  }
  
  if (!supported) {
    die("compatible: tensor strategy '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
  
  compatible = (lhs->n == rhs->l);
  
  if (!compatible) {
    print_information(lhs);
    print_information(rhs);
    die("Tensor and vector do not have matching dimensions.\n");
  }
}

void
compatible(matrix_t const *lhs, tensor_t const *rhs)
{
  bool compatible, supported;
  
  debug("compatible(matrix=0x%x, tensor=0x%x)\n", lhs, rhs);
  
  switch (rhs->strategy) {
  case strategy::compressed:
  case strategy::slice:
  case strategy::ekmr:
  case strategy::zzekmr:
    supported = true;
    break;
  default:
    supported = false;
    break;
  }
  
  if (!supported) {
    die("Tensor strategy '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
  
  switch (rhs->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
  case orientation::lateral:
  case orientation::horizontal:
  case orientation::frontal:
    compatible = true;
    break;
  default:
    compatible = false;
    break;
  }
  
  if (!compatible) {
    print_information(lhs);
    print_information(rhs);
    die("Tensors and matrix do not have matching dimensions.\n");
  }
}

void
compatible(tensor_t const *lhs, tensor_t const *rhs)
{
  debug("compatible(tensor=0x%x, tensor=0x%x)\n", lhs, rhs);
  
  if (lhs->l != rhs->l || lhs->m != rhs->m || lhs->n != rhs->n) {
    die("Tensors do not have the same dimensions.\n");
  }
  
#if 0
  if (rhs->strategy != strategy::coordinate) {
    die("Conversion from '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
#endif
}
