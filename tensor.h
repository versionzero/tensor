
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include "types.h"
#include <stdio.h>

typedef struct {
  uint               l, m, n;
  ownership_t        owner;
  storage_strategy_t strategy;
  tensor_storage_t   *storage;
} tensor_t;

tensor_t *tensor_new(uint l, uint m, uint n, ownership_t owner = creator);
void tensor_delete(tensor_t *tensor);

tensor_t *tensor_read(char const *filename);
tensor_t *tensor_fread(FILE *stream);

#if 0
void tensor_clear(tensor_t *m1);
tensor_t *tensor_copy_shallow(tensor_t const *m1);
void tensor_copy_shallow_inplace(tensor_t *mr, tensor_t const *m1);
void tensor_copy_inplace(tensor_t *mr, tensor_t const *m1);
void tensor_copy_inplace_with_offset(tensor_t *mr, uint oi, uint oj, 
				     tensor_t const *m1);
tensor_t* tensor_copy(tensor_t const *m1);
tensor_t* tensor_partition(tensor_t const *m1, uint i, uint j, uint m, uint n);
void tensor_partition_inplace(tensor_t *mr, tensor_t const *m1, 
			      uint i, uint j, uint m, uint n);

void tensor_write(char const *filename, tensor_t const *m,
		  uint coordinate = 0);
void tensor_fwrite(FILE *stream, tensor_t const *m, uint coordinate = 0);

void tensor_supported(tensor_t const *m1);
void tensor_compatible(tensor_t const *m1, tensor_t const *m2);
void tensor_check_range(tensor_t const *m1, uint i, uint j, uint m, uint n);

void tensor_add_inplace(tensor_t *m1, tensor_t const *m2);
tensor_t *tensor_add(tensor_t const *m1, tensor_t const *m2);

void tensor_subtract_inplace(tensor_t *m1, tensor_t const *m2);
tensor_t *tensor_subtract(tensor_t const *m1, tensor_t const *m2);

void tensor_multiply_inplace(tensor_t *mr, tensor_t const *m1, tensor_t const *m2);
tensor_t *tensor_multiply(tensor_t const *m1, tensor_t const *m2);
tensor_t *tensor_multiply_strassen(tensor_t const *m1, tensor_t const *m2);
tensor_t *tensor_distributed_multiply_strassen(tensor_t const *m1, tensor_t const *m2);
#endif

#endif

