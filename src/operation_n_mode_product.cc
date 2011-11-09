
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

extern cache_t			 *cache;
extern uint			 memory_stride;
extern uint			 thread_count;
extern thread::partition::type_t thread_partition;

static pthread_mutex_t           tube_lock;

/*
  Computing ($pT$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to l do
    for j = 1 to m do 
      for k = 1 to m do
        M[i][j] += p[k] * T[i][j][k]
      end for
    end for
  end for
*/

typedef struct {
  uint           done;
  matrix_t       *matrix;
  vector_t const *vector;
  tensor_t const *tensor;
} product_thread_data_t;

int
tube_next(product_thread_data_t *data)
{
  uint k;
  
  thread_mutex_lock(&tube_lock);
  k = data->done++;
  thread_mutex_unlock(&tube_lock);
  return k < (data->tensor->n*data->tensor->n) ? k : -1;
}

thread_address_t
tube_product(thread_argument_t *argument)
{
  int                   t;
  uint                  i, j, k, offset;
  uint                  n, sum;
  uint                  *P;
  double                **M, *T;
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  n = data->tensor->n;
  M = data->matrix->data;
  P = data->vector->data;
  T = data->tensor->values;
  
  while (-1 != (t = tube_next(data))) {
    sum    = 0;
    offset = t*n;
    i      = t/n;
    j      = t%n;
    for (k = 0; k < n; ++k) {
      sum += P[k] * T[offset+k];
    }
    M[i][j] = sum;
  }
  
  return NULL;
}

int
slice_next(product_thread_data_t *data)
{
  uint k;
  
  thread_mutex_lock(&tube_lock);
  k = data->done++;
  thread_mutex_unlock(&tube_lock);
  return k < (data->tensor->n) ? k : -1;
}

thread_address_t
slice_product(thread_argument_t *argument)
{
  int                   i;
  uint                  j, k;
  uint                  ioffset, joffset;
  uint                  n, sum[1000];
  uint                  *P;
  double                **M, *T;
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  n = data->tensor->n;
  M = data->matrix->data;
  P = data->vector->data;
  T = data->tensor->values;
  
  while (-1 != (i = slice_next(data))) {
    ioffset = i*n*n;
    for (j = 0; j < n; ++j) {
      sum[j]  = 0;
      joffset = ioffset+j*n;
      for (k = 0; k < n; ++k) {
	sum[j] += P[k] * T[joffset+k];
      }
    }
    for (j = 0; j < n; ++j) {
      M[i][j] = sum[j];
    }
  }
  
  return NULL;
}

void
threaded_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, thread_function_t function)
{
  product_thread_data_t data;
  
  memory_stride = memory_stride > tensor->n ? tensor->n : memory_stride;
  thread_count  = thread_count > tensor->n ? tensor->n : thread_count;
  
  debug("threaded_n_mode_product_array: memory_stride=%d\n", memory_stride);
  debug("threaded_n_mode_product_array: thread_count=%d\n", thread_count);
  
  data.done   = 0;
  data.matrix = matrix;
  data.vector = vector;
  data.tensor = tensor;

  thread_mutex_init(&tube_lock);
  thread_fork(thread_count, slice_product, &data, NULL);
  thread_mutex_destroy(&tube_lock);
}

void
threaded_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  thread_function_t function;
  
  switch (thread_partition) {
  case thread::partition::tube:
    function = (thread_function_t) &tube_product;
    break;
  case thread::partition::slice:
    function = (thread_function_t) &slice_product;
    break;
  default:
    die("serial_n_mode_product: tensor product for '%s' strategy is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
  
  threaded_n_mode_product_array(matrix, vector, tensor, function);
}
 
void
serial_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint   i, j, k, index;
  uint   n;
  uint   *P;
  double **M, *T;
  
  debug("n_mode_product_array(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  n = tensor->n;
  M = matrix->data;
  P = vector->data;
  T = tensor->values;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      for (k = 0; k < n; ++k) {
	index = tensor_index(tensor, i, j, k);
	M[i][j] += P[k] * T[index];
      }
    }
  }
}

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
    serial_n_mode_product_array(matrix, vector, tensor);
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
