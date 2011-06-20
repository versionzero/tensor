
#include "error.h"
#include "information.h"

void
print_information(vector_t const* vector)
{
  debug("print_information(matrix=0x%x)\n", vector);
  debug("n=%d\n", vector->n);
}

void
print_information(matrix_t const* matrix)
{
  debug("print_information(matrix=0x%x)\n", matrix);
  debug("m=%d, n=%d\n", matrix->m, matrix->n);
}

void
print_information(tensor_t const* tensor)
{
  debug("print_information(tensor=0x%x)\n", tensor);
  debug("l=%d, m=%d, n=%d, nnz=%d\n", tensor->l, tensor->m, tensor->n, tensor->nnz);
  debug("strategy='%s', orientation='%s'\n", strategy_to_string(tensor->strategy), orientation_to_string(tensor->orientation));
}
