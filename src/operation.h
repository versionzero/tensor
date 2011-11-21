
#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "matrix.h"
#include "tensor.h"
#include "vector.h"

namespace operation {
  typedef enum {
    unknown,
    mode_1_product
  } type_t;
}

char const* operation_to_string(operation::type_t operation);
char const* operation_to_description_string(operation::type_t operation);
operation::type_t string_to_operation(char const *name);

void operation_mode_1_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor);
matrix_t *operation_mode_1_product(vector_t const *vector, tensor_t const *tensor);

void serial_mode_1_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor);
void threaded_mode_1_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
