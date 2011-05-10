
#ifndef _TENSOR_STORAGE_H_
#define _TENSOR_STORAGE_H_

#include "types.h"

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

void *tensor_storage_coordinate_strategy_new(uint nnz);
void *tensor_storage_ekmr_strategy_new(uint nnz);

#endif

