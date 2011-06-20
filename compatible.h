
#ifndef _COMPATIBLE_H_
#define _COMPATIBLE_H_

#include "matrix.h"
#include "tensor.h"
#include "vector.h"

void compatible(tensor_t const *lhs, tensor_t const *rhs);
void compatible(vector_t const *lhs, tensor_t const *rhs);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
