
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "thread.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

extern uint thread_count;

void
threaded_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("threaded_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::array:
    threaded_n_mode_product_array(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' strategy (using thread_count) is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}

void
serial_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("serial_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::array:
    /* in this case, we want to compare the single thread version of
       the same algo against the n-threaded version */
    threaded_n_mode_product_array(matrix, vector, tensor);
    break;
  default:
    die("serial_n_mode_product: tensor product for '%s' strategy is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}

void
operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("operation_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  if (thread_count <= 1) {
    serial_n_mode_product(matrix, vector, tensor);
  } else {
    threaded_n_mode_product(matrix, vector, tensor);
  }
}

matrix_t*
operation_n_mode_product(vector_t const *vector, tensor_t const *tensor)
{
  matrix_t *matrix;
  
  compatible(vector, tensor);
  debug("operation_n_mode_product(vector=0x%x, tensor=0x%x)\n", vector, tensor);
  
  matrix = matrix_malloc(tensor->m, tensor->n, ownership::creator);
  debug("operation_n_mode_product: matrix=0x%x\n", matrix);
 
  operation_n_mode_product(matrix, vector, tensor);
  
  return matrix;
}
