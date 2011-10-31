
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "types.h"
#include <stdio.h>

namespace format {
  typedef enum {
    format,
    array,
    coordinate
  } type_t;
}

namespace scheme {
  typedef enum {
    general,
    symmetric
  } type_t;
}

typedef struct {
  uint              m, n;
  ownership::type_t owner;
  scheme::type_t    scheme;
  double            **data;
} matrix_t;

typedef struct {
  uint              m, n, rn;
  ownership::type_t owner;
  scheme::type_t    scheme;
  uint              *RO, *CO;
  double            *values;
} smatrix_t;

matrix_t *matrix_malloc(uint m, uint n, ownership::type_t owner = ownership::creator);
void matrix_free(matrix_t *m);
void matrix_clear(matrix_t *m1);
matrix_t *matrix_copy_shallow(matrix_t const *m1);
void matrix_copy_shallow(matrix_t *mr, matrix_t const *m1);
void matrix_copy(matrix_t *mr, matrix_t const *m1);
void matrix_copy_with_offset(matrix_t *mr, matrix_t const *m1, uint oi, uint oj);
matrix_t* matrix_copy(matrix_t const *m1);
matrix_t* matrix_partition(matrix_t const *m1, uint i, uint j, uint m, uint n);
void matrix_partition(matrix_t *mr, matrix_t const *m1, uint i, uint j, uint m, uint n);

matrix_t *matrix_read(char const *filename);
matrix_t *matrix_fread(FILE *stream);
matrix_t* matrix_fread_data(FILE *f, MM_typecode type);
void matrix_write(char const *filename, matrix_t const *m, format::type_t format = format::array);
void matrix_fwrite(FILE *stream, matrix_t const *m, format::type_t format = format::array);

void matrix_supported(matrix_t const *m1);
void matrix_compatible(matrix_t const *m1, matrix_t const *m2);
void matrix_check_range(matrix_t const *m1, uint i, uint j, uint m, uint n);

void matrix_add(matrix_t *m1, matrix_t const *m2);
matrix_t *matrix_add(matrix_t const *m1, matrix_t const *m2);

void matrix_subtract(matrix_t *m1, matrix_t const *m2);
matrix_t *matrix_subtract(matrix_t const *m1, matrix_t const *m2);

void matrix_multiply(matrix_t *mr, matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_multiply(matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_multiply_strassen(matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_distributed_multiply_strassen(matrix_t const *m1, matrix_t const *m2);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
