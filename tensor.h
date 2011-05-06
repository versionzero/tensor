
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include "types.h"
#include <stdio.h>

typedef enum {
  cooridinate,
  ekmr,
} storage_strategy_t;

typedef enum {
  row,
  column,
  slice
} minor_storage_strategy_t;

typedef void tensor_storage_t;

typedef struct {
  uint               l, m, n;
  ownership_t        owner;
  storage_strategy_t strategy;
  tensor_storage_t   *data;
} tensor_t;

typedef struct {
  uint   *I, *J, *K;
  double *values;
} cooridinate_storage_t;

typedef struct {
  uint   *I, *J;
  double *values;
} ekmr_storage_t;

#define COORIDINATE_DATA(x) ((cooridinate_storage_t*)x->data)
#define EKMR_DATA(x) ((ekmr_storage_t*)x->data)

tensor_t *tensor_new(uint l, uint m, uint n, ownership_t owner = creator);
tensor_t *tensor_new_or_die(uint l, uint m, uint n, ownership_t owner = creator);
void tensor_delete(tensor_t *t);

tensor_t *tensor_read(char const *filename);
tensor_t *tensor_fread(FILE *stream);

void tensor_clear(tensor_t *t);
tensor_t *tensor_copy_shallow(tensor_t const *t1);
void tensor_copy_shallow_inplace(tensor_t *tr, tensor_t const *t1);
void tensor_copy_inplace(tensor_t *tr, tensor_t const *t1);
void tensor_copy_inplace_with_offset(tensor_t *tr, tensor_t const *t1, uint oi, uint oj);
tensor_t *tensor_copy(tensor_t const *t1);
tensor_t *tensor_partition(tensor_t const *t1, uint i, uint j, uint k, uint m, uint n, uint l);
void tensor_partition_inplace(tensor_t *tr, tensor_t const *t1, uint i, uint j, uint j, uint m, uint n, uint l);

void tensor_write(char const *filename, tensor_t const *t, bool coordinate = false);
void tensor_fwrite(FILE *stream, tensor_t const *t, uint coordinate = 0);

void tensor_supported(tensor_t const *t1);
void tensor_compatible(tensor_t const *t1, tensor_t const *t2);
void tensor_check_range(tensor_t const *t1, uint i, uint j, uint m, uint n);

void tensor_add_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_add(tensor_t const *t1, tensor_t const *t2);

void tensor_subtract_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_subtract(tensor_t const *t1, tensor_t const *t2);

void tensor_multiply_inplace(tensor_t *tr, tensor_t const *t1, tensor_t const *t2);
tensor_t *tensor_multiply(tensor_t const *t1, tensor_t const *t2);


#endif

