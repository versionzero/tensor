
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
  Let $p \in R^{n}$ be a matrix.
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
  tensor_t       *result;
  matrix_t const *matrix;
  tensor_t const *tensor;
} product_thread_data_t;

typedef void (*mode_2_product_t)(product_thread_data_t *data, uint n, double *R, double *M, double *T);

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
slice_consumer_implementation(product_thread_data_t *data, uint n, double *R, double *M, double *T)
{
  int  i;
  uint offset;
  
  while (-1 != (i = slice_next(data))) {
    offset = i*n*n;
    array_matrix_multiply(n, n, n, M, n, T+offset, n, R+offset, n);
  }
}

static
thread_address_t
slice_consumer(thread_argument_t *argument)
{
  product_thread_data_t *data;
  
  data = (product_thread_data_t*) thread_data(argument);
  
  slice_consumer_implementation(data, data->tensor->n, data->result->values, data->matrix->data[0], data->tensor->values);
  
  return NULL;
}


void
operation_mode_2_product_array(tensor_t *result, tensor_t const *tensor, matrix_t const *matrix, thread_function_t producer, thread_function_t consumer)
{
  product_thread_data_t data;
  
  data.done   = 0;
  data.matrix = matrix;
  data.result = result;
  data.tensor = tensor;
  
  if (NULL != producer) {
    thread_create_detached(producer, &data);
  }
  
  thread_afork(thread_count, consumer, &data, NULL);
}

void
operation_mode_2_product_array(tensor_t *result, tensor_t const *tensor, matrix_t const *matrix)
{
  thread_function_t consumer, producer;
  
  producer = NULL;
  consumer = NULL;
  
  switch (data_partition) {
  case data::partition::slice:
    consumer = (thread_function_t) &slice_consumer;
    break;
#if 0
  case data::partition::slice_recursive_block:
    consumer = (thread_function_t) &slice_recursive_block_consumer;
    break;
#endif
  default:
    die("operation_mode_2_product_array: tensor product for '%s' partition is not currently supported.\n",
	data_partition_to_string(data_partition));
    break;
  }
  
  operation_mode_2_product_array(result, tensor, matrix, producer, consumer);
}
 
