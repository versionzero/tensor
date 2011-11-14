
#include "algebra.h"
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

extern association::type_t       operand_association;
extern cache_t			 *cache;
extern uint			 memory_stride;
extern uint			 thread_count;
extern thread::partition::type_t thread_partition;

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
  
  Computing ($Tp$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to l do
    for j = 1 to m do 
      for k = 1 to m do
        M[i][j] += T[j][i][k] * p[k]
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
  volatile uint k;
  
  /* rather than a lock we can take advantage of the architecture and
     issue an atomic fetch and increment */
  k = __sync_fetch_and_add(&data->done, 1);
  return k < (data->tensor->n*data->tensor->n) ? k : -1;
}

void
tube_product_pT(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  t;
  uint i, j, offset;
  
  while (-1 != (t = tube_next(data))) {
    offset  = t*n;
    i       = t/n;
    j       = t%n;
    M[i][j] = array_inner_product(n, P, 1, T+offset, 1);
  }
}

void
tube_product_Tp(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  t;
  uint i, j, offset;
  
  while (-1 != (t = tube_next(data))) {
    offset  = t*n;
    i       = t/n;
    j       = t%n;
    M[i][j] = array_inner_product(n, P, 1, T+offset, 1);
  }
}

thread_address_t
tube_product(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  if (association::left == operand_association) {
    tube_product_pT(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  } else {
    tube_product_Tp(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  }
  
  return NULL;
}

int
slice_next(product_thread_data_t *data)
{
  volatile uint k;
  
  /* rather than a lock we can take advantage of the architecture and
     issue an atomic fetch and increment */
  k = __sync_fetch_and_add(&data->done, 1);
  return k < data->tensor->n ? k : -1;
}

void
slice_product_pT(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  i;
  uint j, ioffset, joffset;
  
  while (-1 != (i = slice_next(data))) {
    ioffset = i*n*n;
    for (j = 0; j < n; ++j) {
      joffset = ioffset+j*n;
      M[i][j] = array_inner_product(n, P, 1, T+joffset, 1);
    }
  }
}

void
slice_product_Tp(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  i;
  uint j, ioffset, joffset;
  
  while (-1 != (i = slice_next(data))) {
    ioffset = i*n*n;
    for (j = 0; j < n; ++j) {
      joffset = ioffset+j*n;
      M[i][j] = array_inner_product(n, P, 1, T+joffset, 1);
    }
  }
}

thread_address_t
slice_product(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  if (association::left == operand_association) {
    slice_product_pT(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  } else {
    slice_product_Tp(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
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
  
  thread_afork(thread_count, function, &data, NULL);
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
  uint   i, j, k;
  uint   index, sum;
  uint   n;
  double **M, *T, *P;
  
  debug("n_mode_product_array(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  n = tensor->n;
  M = matrix->data;
  P = vector->data;
  T = tensor->values;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      sum = 0;
      for (k = 0; k < n; ++k) {
	index = tensor_index(tensor, i, j, k);
	sum += P[k] * T[index];
      }
      M[i][j] = sum;
    }
  }
}
