
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

typedef void (*mode_1_product_t)(product_thread_data_t *data, uint n, double **M, double *P, double *T);

static
int
fiber_next(product_thread_data_t *data)
{
  volatile uint k;
  
  /* rather than a lock we can take advantage of the architecture and
     issue an atomic fetch and increment */
  k = __sync_fetch_and_add(&data->done, 1);
  return k < (data->tensor->n*data->tensor->n) ? k : -1;
}

static
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

static
thread_address_t
fiber_consumer(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  fiber_consumer_implementation(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  
  return NULL;
}

static
void
fiber_block_consumer_implementation(product_thread_data_t *data, uint n, uint start, uint end, double **M, double *P, double *T)
{
  uint i, j, t, offset;
  
  for (t = start; t < end; ++t) {
    offset  = t*n;
    i       = t/n;
    j       = t%n;
    M[i][j] = array_inner_product(n, P, 1, T+offset, 1);
  }
}

static
thread_address_t
fiber_block_consumer(thread_argument_t *argument)
{
  int                   id;
  uint                  n, block_size;
  uint                  start, end;
  product_thread_data_t *data;
  
  data       = (product_thread_data_t*) thread_data(argument);
  n          = data->tensor->n;
  id         = thread_myid(argument);
  block_size = (n*n)/thread_count;
  start      = block_size*id;
  end        = start+block_size;
  
  fiber_block_consumer_implementation(data, n, start, end, data->matrix->data, data->vector->data, data->tensor->values);
  
  return NULL;
}

static
int
slice_next(product_thread_data_t *data)
{
  volatile uint k;
  
  /* rather than a lock we can take advantage of the architecture and
     issue an atomic fetch and increment */
  k = __sync_fetch_and_add(&data->done, 1);
  return k < data->tensor->n ? k : -1;
}

static
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

static
thread_address_t
slice_consumer(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  slice_consumer_implementation(data, data->tensor->n, data->matrix->data, data->vector->data, data->tensor->values);
  
  return NULL;
}

static
void
slice_block_consumer_implementation(int id, product_thread_data_t *data, uint n, uint start, uint end, double **M, double *P, double *T)
{
  uint i, j, ioffset, joffset;
  
  for (i = start; i < end; ++i) {
    ioffset = i*n*n;
    joffset = ioffset;
    for (j = 0; j < n; ++j) {
      M[i][j]  = array_inner_product(n, P, 1, T+joffset, 1);
      joffset += n;
    }
  }
}

static
thread_address_t
slice_block_consumer(thread_argument_t *argument)
{
  int                   id;
  uint                  n, block_size;
  uint                  start, end;
  product_thread_data_t *data;
  
  data       = (product_thread_data_t*) thread_data(argument);
  n          = data->tensor->n;
  id         = thread_myid(argument);
  block_size = n/thread_count;
  start      = block_size*id;
  end        = start+block_size;
  
  DEBUG("thread:%d: block_size=%d/%d=%d, start=%d, end=%d\n", id, n, thread_count, block_size, start, end);
  
  slice_block_consumer_implementation(id, data, n, start, end, data->matrix->data, data->vector->data, data->tensor->values);
  
  return NULL;
}

void
operation_mode_1_product_array(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector, thread_function_t producer, thread_function_t consumer)
{
  product_thread_data_t data;
  
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
operation_mode_1_product_array(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector)
{
  thread_function_t consumer, producer;
  
  producer = NULL;
  consumer = NULL;
  
  switch (data_partition) {
  case data::partition::fiber:
    consumer = (thread_function_t) &fiber_consumer;
    break;
  case data::partition::fiber_block:
    consumer = (thread_function_t) &fiber_block_consumer;
    break;
  case data::partition::slice:
    consumer = (thread_function_t) &slice_consumer;
    break;
  case data::partition::slice_block:
    consumer = (thread_function_t) &slice_block_consumer;
    break;
  default:
    die("threaded_mode_1_product_array: tensor product for '%s' partition is not currently supported.\n",
	data_partition_to_string(data_partition));
    break;
  }
  
  operation_mode_1_product_array(matrix, tensor, vector, producer, consumer);
}
 
void
serial_mode_1_product_array(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector)
{
  uint   i, j, k;
  uint   index, sum;
  uint   n;
  double **M, *T, *P;
  
  debug("mode_1_product_array(matrix=0x%x, tensor=0x%x, vector=0x%x)\n", matrix, tensor, vector);
  
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
