
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "tensor.h"

void* tensor_storage_malloc(tensor_t const *tensor);
tensor_storage_coordinate_t* tensor_storage_malloc_coordinate(tensor_t const *tensor);
tensor_storage_compressed_t* tensor_storage_malloc_compressed(tensor_t const *tensor);
tensor_storage_compressed_t* tensor_storage_malloc_compressed_slice(tensor_t const *tensor);
tensor_storage_extended_t* tensor_storage_malloc_ekmr(tensor_t const *tensor);
tensor_storage_extended_t* tensor_storage_malloc_zzekmr(tensor_t const *tensor);

void tensor_storage_convert(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_compressed(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_compressed_slice(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_gundersen(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_ekmr(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_coordinate_to_zzekmr(tensor_t *destination, tensor_t *source);
void tensor_storage_convert_from_compressed_to_coordinate(tensor_t *destination, tensor_t *source);

int index_compare_ijk(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);
int index_compare_jik(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);
int index_compare_jki(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);
int index_compare_kji(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);
int index_compare_kij(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);
int index_compare_ikj(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb);

uint tensor_storage_index_encode(uint *indices, coordinate_tuple_t const *tuple, uint nnz, index_encoder_t encoder);
uint encoder_for_i(coordinate_tuple_t const *tuple);
uint encoder_for_j(coordinate_tuple_t const *tuple);
uint encoder_for_k(coordinate_tuple_t const *tuple);

void tensor_storage_copy(void *destination, void const *source, uint nnz, index_copy_t copier);
void copier_for_i(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i);
void copier_for_j(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i);
void copier_for_k(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i);
void copier_for_values(tensor_t *destination, tensor_t const *source, uint i);


#endif

