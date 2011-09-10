
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

void
matrix_initialize_type(MM_typecode *type)
{
  mm_initialize_typecode(type);
  datatype_to_typecode(type, datatype::matrix);
  mm_set_real(type);
}

void
matrix_fwrite_array(FILE *file, matrix_t const *matrix)
{
  uint        i, j;
  int         result;
  MM_typecode type;
  
  matrix_initialize_type(&type);
  mm_set_array(&type);
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Matrix Market banner (%d).\n", result);
  }
  
  if (0 != (result = mm_write_matrix_array_size(file, matrix->m, matrix->n))) {
    die("Failed to write matrix array size (%d).\n", result);
  }

  for (i = 0; i < matrix->m; ++i) {
    for (j = 0; j < matrix->n; ++j) {
      fprintf(file, "%10.32g\n", matrix->data[i][j]);
    }
  }
}

void
matrix_fwrite_coordinate(FILE *file, matrix_t const *matrix)
{
  uint        i, j;
  int         nnz, result;
  MM_typecode type;
  
  matrix_initialize_type(&type);
  mm_set_coordinate(&type);
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Matrix Market banner (%d).\n", result);
  }
  
  nnz = 0;
  for (i = 0; i < matrix->m; ++i) {
    for (j = 0; j < matrix->n; ++j) {
      if (0.0 != matrix->data[i][j]) {
	nnz++;
      }
    }
  }
  
  if (0 != (result = mm_write_matrix_coordinate_size(file, matrix->m, matrix->n, nnz))) {
    die("Failed to write matrix coordinate size %d (%d).\n", nnz, result);
  }

  for (i = 0; i < matrix->m; ++i) {
    for (j = 0; j < matrix->n; ++j) {
      if (0.0 != matrix->data[i][j]) {
	fprintf(file, "%d %d %10.32g\n", i+1, j+1, matrix->data[i][j]);
      }
    }
  }
}

void
matrix_fwrite(FILE *file, matrix_t const *matrix, format::type_t format)
{
  if (format::coordinate == format) {
    matrix_fwrite_coordinate(file, matrix);
  } else {
    matrix_fwrite_array(file, matrix);
  }
}

void
matrix_write(char const *filename, matrix_t const *matrix, format::type_t format)
{
  FILE *file;

  file = fopen_or_die(filename, "w+");
  matrix_fwrite(file, matrix, format);
  fclose(file);
}

