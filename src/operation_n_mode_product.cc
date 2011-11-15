
#include "algebra.h"
#include "data.h"
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "queue.h"
#include "thread.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

extern cache_t		       *cache;
extern uint		       memory_stride;
extern orientation::type_t     storage_orientation;
extern strategy::type_t        storage_strategy;
extern uint		       thread_count;
extern data::partition::type_t data_partition;

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

typedef void (*n_mode_product_t)(product_thread_data_t *data, uint n, double **M, double *P, double *T);

int
fiber_next(product_thread_data_t *data)
{
  volatile uint k;
  
  /* rather than a lock we can take advantage of the architecture and
     issue an atomic fetch and increment */
  k = __sync_fetch_and_add(&data->done, 1);
  return k < (data->tensor->n*data->tensor->n) ? k : -1;
}

void
fiber_consumer_implementation(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  t;
  uint i, j, offset;
  
  while (-1 != (t = fiber_next(data))) {
    offset  = t*n;
    i       = t/n;
    j       = t%n;
    M[i][j] = array_inner_product(n, P, 1, T+offset, 1);
  }
}

thread_address_t
fiber_consumer(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  fiber_consumer_implementation(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  
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
slice_consumer_implementation(product_thread_data_t *data, uint n, double **M, double *P, double *T)
{
  int  i;
  uint j, ioffset, joffset;
  
  while (-1 != (i = slice_next(data))) {
    ioffset = i*n*n;
    joffset = ioffset;
    for (j = 0; j < n; ++j) {
      M[i][j]  = array_inner_product(n, P, 1, T+joffset, 1);
      joffset += n;
    }
  }
}

thread_address_t
slice_consumer(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  slice_consumer_implementation(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  
  return NULL;
}

void
threaded_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, thread_function_t producer, thread_function_t consumer)
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
  
  if (NULL != producer) {
    thread_create_detached(producer, &data);
  }
  
  thread_afork(thread_count, consumer, &data, NULL);
}

void
threaded_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  thread_function_t consumer, producer;
  
  producer = NULL;
  consumer = NULL;
  
  switch (data_partition) {
  case data::partition::fiber:
    consumer = (thread_function_t) &fiber_consumer;
    break;
  case data::partition::slice:
    consumer = (thread_function_t) &slice_consumer;
    break;
  case data::partition::fiber_decomposition:
    consumer = (thread_function_t) &subfiber_consumer;
    producer = (thread_function_t) &subfiber_producer;
  default:
    die("threaded_n_mode_product_array: tensor product for '%s' partition is not currently supported.\n",
	data_partition_to_string(data_partition));
    break;
  }
  
  threaded_n_mode_product_array(matrix, vector, tensor, producer, consumer);
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
