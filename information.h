
#ifndef _INFORMATION_H_
#define _INFORMATION_H_

#include "matrix.h"
#include "tensor.h"
#include "vector.h"

void print_information(vector_t const* vector);
void print_information(matrix_t const* matrix);
void print_information(tensor_t const* tensor);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
