
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include "mmio.h"
#include "types.h"
#include "vector.h"
#include <stdio.h>

namespace file_format {
  typedef enum {
    unknown,
    mmio,
    matlab
  } type_t;
}

namespace strategy {
  typedef enum {
    unknown,
    array,
    coordinate,
    compressed,
    gundersen,
    slice,
    ekmr,
    zzekmr
  } type_t;
}

namespace orientation {
  typedef enum {
    unknown,
    row,
    column,
    tube,
    lateral,
    horizontal,
    frontal
  } type_t;
}

typedef struct {
  uint                l, m, n;
  strategy::type_t    strategy;
  orientation::type_t orientation;
  ownership::type_t   owner;
  double              *values;
} tensor_t;

typedef struct {
  uint i, j, k;
  uint index;
} coordinate_tuple_t;

tensor_t* tensor_malloc(uint l, uint m, uint n, ownership::type_t owner = ownership::creator);
tensor_t* tensor_malloc(uint l, uint m, uint n, strategy::type_t strategy, 
			orientation::type_t orientation = orientation::unknown,
			ownership::type_t owner = ownership::creator);
tensor_t* tensor_malloc_from_template(tensor_t const *tensor);
uint tensor_index(tensor_t const *tensor, uint i, uint j, uint k);
void tensor_free(tensor_t *tensor);

tensor_t* tensor_copy_shallow(tensor_t *source);
void tensor_copy_shallow(tensor_t *destination, tensor_t *source);
void tensor_transfer_ownership(tensor_t *destination, tensor_t *source);

void tensor_clear(tensor_t *tensor);

#if 0
tensor_t *tensor_convert(tensor_t *tensor, strategy::type_t strategy, orientation::type_t orientation = orientation::unknown);
void tensor_convert(tensor_t *destination, tensor_t *source);
#endif

tensor_t *tensor_read(char const *filename);
tensor_t *tensor_fread(FILE *file);
tensor_t *tensor_fread_data(FILE *file, MM_typecode type);
void tensor_write(char const *filename, tensor_t const *tensor);
void tensor_fwrite(FILE *file, tensor_t const *tensor);
void tensor_emit_latex(FILE *file, tensor_t const *tensor);

void tensor_supported(tensor_t const *t1);
void tensor_validate(tensor_t const *tensor);

char const* strategy_to_string(strategy::type_t strategy);
char const* orientation_to_string(orientation::type_t orientation);
char const* orientation_to_latex_macro(orientation::type_t orientation);
strategy::type_t string_to_strategy(char const *name);
orientation::type_t string_to_orientation(char const *name);
strategy::type_t typecode_to_strategy(MM_typecode type);
void strategy_to_typecode(MM_typecode *type, strategy::type_t strategy);
void print_strategies(char const *format);
void print_orientations(char const *format);
void print_operations(char const *format);
void print_operations_with_descriptions(char const *format);

#if 0
void tensor_add(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_add(tensor_t const *t1, tensor_t const *t2);

void tensor_subtract(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_subtract(tensor_t const *t1, tensor_t const *t2);

void tensor_multiply(tensor_t *tr, tensor_t const *t1, tensor_t const *t2);
tensor_t *tensor_multiply(tensor_t const *t1, tensor_t const *t2);
#endif

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
