
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "types.h"
#include <stdio.h>

typedef struct {
  uint        m, n;
  ownership_t owner;
  double      **data;
} matrix_t;

matrix_t *matrix_new(uint m, uint n, ownership_t owner = creator);
void matrix_delete(matrix_t *m);
void matrix_clear(matrix_t *m1);
matrix_t *matrix_copy_shallow(matrix_t const *m1);
void matrix_copy_shallow_inplace(matrix_t *mr, matrix_t const *m1);
void matrix_copy_inplace(matrix_t *mr, matrix_t const *m1);
void matrix_copy_inplace_with_offset(matrix_t *mr, matrix_t const *m1, uint oi, uint oj);
matrix_t* matrix_copy(matrix_t const *m1);
matrix_t* matrix_partition(matrix_t const *m1, uint i, uint j, uint m, uint n);
void matrix_partition_inplace(matrix_t *mr, matrix_t const *m1, uint i, uint j, uint m, uint n);

matrix_t *matrix_read(char const *filename);
matrix_t *matrix_fread(FILE *stream);
void matrix_write(char const *filename, matrix_t const *m, uint coordinate = 0);
void matrix_fwrite(FILE *stream, matrix_t const *m, uint coordinate = 0);

void matrix_supported(matrix_t const *m1);
void matrix_compatible(matrix_t const *m1, matrix_t const *m2);
void matrix_check_range(matrix_t const *m1, uint i, uint j, uint m, uint n);

void matrix_add_inplace(matrix_t *m1, matrix_t const *m2);
matrix_t *matrix_add(matrix_t const *m1, matrix_t const *m2);

void matrix_subtract_inplace(matrix_t *m1, matrix_t const *m2);
matrix_t *matrix_subtract(matrix_t const *m1, matrix_t const *m2);

void matrix_multiply_inplace(matrix_t *mr, matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_multiply(matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_multiply_strassen(matrix_t const *m1, matrix_t const *m2);
matrix_t *matrix_distributed_multiply_strassen(matrix_t const *m1, matrix_t const *m2);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
