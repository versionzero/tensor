
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include "storage.h"
#include "types.h"
#include <stdio.h>

typedef enum {
  coordinate,
  ekmr,
} storage_strategy_t;

typedef enum {
  row,
  column,
  slice
} minor_storage_strategy_t;

typedef struct {
  uint               l, m, n;
  storage_strategy_t strategy;
  void               *storage;
} tensor_t;

typedef struct {
  uint   nnz;
  uint   *I, *J, *K;
  double *values;
} storage_coordinate_t;

typedef struct {
  uint   *I, *J;
  double *values;
} storage_ekmr_t;

#define STORAGE_COORIDINATE(x) ((storage_coordinate_t*)x->storage)
#define STORAGE_EKMR(x) ((storage_ekmr_t*)x->storage)

tensor_t *tensor_new(uint l, uint m, uint n, uint nnz = 0, storage_strategy_t strategy = coordinate);
void tensor_delete(tensor_t *tensor);
void tensor_clear(tensor_t *tensor);
void tensor_copy_inplace(tensor_t *tr, tensor_t const *tensor);
tensor_t *tensor_copy(tensor_t const *tensor);

char const* tensor_storage_name(storage_strategy_t strategy);

tensor_t *tensor_read(char const *filename);
tensor_t *tensor_fread(FILE *file);
void tensor_write(char const *filename, tensor_t const *tensor);
void tensor_fwrite(FILE *file, tensor_t const *tensor);

void tensor_supported(tensor_t const *t1);
void tensor_compatible(tensor_t const *t1, tensor_t const *t2);

#if 0
void tensor_add_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_add(tensor_t const *t1, tensor_t const *t2);

void tensor_subtract_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_subtract(tensor_t const *t1, tensor_t const *t2);

void tensor_multiply_inplace(tensor_t *tr, tensor_t const *t1, tensor_t const *t2);
tensor_t *tensor_multiply(tensor_t const *t1, tensor_t const *t2);
#endif

#endif

