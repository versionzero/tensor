
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include "mmio.h"
#include "types.h"
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
  uint                l, m, n, nnz;
  strategy::type_t    strategy;
  orientation::type_t orientation;
  void                *storage;
  double              *values;
} tensor_t;

typedef struct {
  uint i, j, k;
  uint index;
} coordinate_tuple_t;

typedef int  (*index_compare_t)(const void *a, const void *b);
typedef uint (*index_encoder_t)(coordinate_tuple_t const *tuple);
typedef void (*index_copy_t)(void *destination, void const *source, uint i);

typedef struct {
  index_compare_t index_compare;
  index_encoder_t index_r_encoder;
  index_encoder_t index_c_encoder;
  index_copy_t    index_copy;
} conversion_callbacks_t;

typedef struct {
  conversion_callbacks_t *callbacks;
} tensor_storage_base_t;

typedef struct {
  tensor_storage_base_t dummy;
  coordinate_tuple_t    *tuples;
} tensor_storage_coordinate_t;

typedef struct {
  tensor_storage_base_t base;
  uint                  rn, cn, tn, kn;
  uint                  *RO, *CO, *TO, *KO;
} tensor_storage_compressed_t;

typedef struct {
  tensor_storage_base_t base;
  uint                  rn, ckn;
  uint                  *RO, *CK;
} tensor_storage_extended_t;

typedef struct {
  tensor_storage_extended_t dummy;
} tensor_storage_ekmr_t;

typedef struct {
  tensor_storage_extended_t dummy;
} tensor_storage_zzekmr_t;

#define STORAGE_BASE(x) ((tensor_storage_base_t*)x->storage)
#define STORAGE_COORIDINATE(x) ((tensor_storage_coordinate_t*)x->storage)
#define STORAGE_COMPRESSED(x) ((tensor_storage_compressed_t*)x->storage)
#define STORAGE_EXTENDED(x) ((tensor_storage_extended_t*)x->storage)

tensor_t* tensor_malloc(uint l, uint m, uint n, uint nnz, strategy::type_t strategy, orientation::type_t orientation = orientation::unknown);
tensor_t* tensor_malloc_from_template(tensor_t const *tensor);
void tensor_free(tensor_t *tensor);

void tensor_clear(tensor_t *tensor);

void tensor_convert_inplace(tensor_t *destination, tensor_t *source);
tensor_t *tensor_convert(tensor_t *tensor, strategy::type_t strategy, orientation::type_t orientation = orientation::unknown);

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
void tensor_add_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_add(tensor_t const *t1, tensor_t const *t2);

void tensor_subtract_inplace(tensor_t *t1, tensor_t const *t2);
tensor_t *tensor_subtract(tensor_t const *t1, tensor_t const *t2);

void tensor_multiply_inplace(tensor_t *tr, tensor_t const *t1, tensor_t const *t2);
tensor_t *tensor_multiply(tensor_t const *t1, tensor_t const *t2);
#endif

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
