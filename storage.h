
#ifndef _TENSOR_STORAGE_H_
#define _TENSOR_STORAGE_H_

#include "types.h"

void *tensor_storage_coordinate_strategy_new(uint nnz);
void *tensor_storage_ekmr_strategy_new(uint nnz);

#endif

