
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "tensor.h"

void* tensor_storage_malloc(tensor_t const *tensor);
tensor_storage_compressed_t* tensor_storage_malloc_compressed(tensor_t const *tensor);
tensor_storage_compressed_t* tensor_storage_malloc_compressed_slice(tensor_t const *tensor);
tensor_storage_extended_t* tensor_storage_malloc_ekmr(tensor_t const *tensor);
tensor_storage_extended_t* tensor_storage_malloc_zzekmr(tensor_t const *tensor);

void tensor_storage_convert_inplace(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_compressed_inplace(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_zzekmr_inplace(tensor_t *destination, tensor_t *source);

uint tensor_storage_index_encode(uint *indices, coordinate_tuple_t const *tuple, uint nnz, index_encoder_t encoder);
uint encoder_for_i(coordinate_tuple_t const *tuple);
uint encoder_for_j(coordinate_tuple_t const *tuple);
uint encoder_for_k(coordinate_tuple_t const *tuple);

#endif

