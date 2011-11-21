
#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "matrix.h"
#include "tensor.h"
#include "vector.h"

namespace operation {
  typedef enum {
    unknown,
    mode_1_product,
    mode_2_product
  } type_t;
}

void operation_mode_1_product(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector);
matrix_t *operation_mode_1_product(tensor_t const *tensor, vector_t const *vector);
void operation_mode_2_product(tensor_t *result, tensor_t const *tensor, matrix_t const *matrix);
tensor_t *operation_mode_2_product(tensor_t const *tensor, matrix_t const *matrix);

void operation_mode_1_product_array(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector);
void operation_mode_2_product_array(tensor_t *result, tensor_t const *tensor, matrix_t const *matrix);

char const* operation_to_string(operation::type_t operation);
char const* operation_to_description_string(operation::type_t operation);
operation::type_t string_to_operation(char const *name);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
