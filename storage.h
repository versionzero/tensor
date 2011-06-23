
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "tensor.h"

void* storage_malloc(tensor_t const *tensor);
storage_compressed_t* storage_malloc_compressed(tensor_t const *tensor);
storage_extended_t* storage_malloc_ekmr(tensor_t const *tensor);
storage_extended_t* storage_malloc_zzpkmr(tensor_t const *tensor);

void storage_convert_inplace(tensor_t *destination, tensor_t *source);
void storage_convert_from_coordinate_to_compressed_inplace(tensor_t *destination, tensor_t *source);
void storage_convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source);
void storage_convert_from_coordinate_to_zzpkmr_inplace(tensor_t *destination, tensor_t *source);

#endif

