
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

#if 0
void
matrix_initialize_type(MM_typecode *type)
{
  mm_initialize_typecode(type);
  mm_set_matrix(type);
  mm_set_real(type);
}

void
matrix_write_array(FILE *f, matrix_t const *m1)
{
  uint        i, j;
  int         result;
  MM_typecode type;
  
  matrix_initialize_type(&type);
  mm_set_array(&type);
  
  if (0 != (result = mm_write_banner(f, type))) {
    error(D_ERROR|D_FATAL, 
	  "Could not write Matrix Market banner (%d).\n",
	  result);
  }
  
  if (0 != (result = mm_write_mtx_array_size(f, m1->m, m1->n))) {
    error(D_ERROR|D_FATAL, 
	  "Failed to write matrix array size (%d).\n", 
	  result);
  }

  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      fprintf(f, "%10.32g\n", m1->data[i][j]);
    }
  }
}

void
matrix_write_coordinate(FILE *f, matrix_t const *m1)
{
  uint        i, j;
  int         nnz, result;
  MM_typecode type;
  
  matrix_initialize_type(&type);
  mm_set_coordinate(&type);
  
  if (0 != (result = mm_write_banner(f, type))) {
    error(D_ERROR|D_FATAL, 
	  "Could not write Matrix Market banner (%d).\n",
	  result);
  }
  
  nnz = 0;
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      if (0.0 != m1->data[i][j]) {
	nnz++;
      }
    }
  }
  
  if (0 != (result = mm_write_mtx_crd_size(f, m1->m, m1->n, nnz))) {
    error(D_ERROR|D_FATAL, 
	  "Failed to write matrix coordinate size %d (%d).\n", 
	  nnz, result);
  }

  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      if (0.0 != m1->data[i][j]) {
	fprintf(f, "%d %d %10.32g\n", i+1, j+1, m1->data[i][j]);
      }
    }
  }
}

void
matrix_fwrite(FILE *f, matrix_t const *m1, uint coordinate)
{
  if (0 == coordinate) {
    matrix_write_array(f, m1);
  } else {
    matrix_write_coordinate(f, m1);
  }
}

void
matrix_write(char const *filename, matrix_t const *m1, uint coordinate)
{
  FILE *f;

  if (NULL == (f = fopen(filename, "w+"))) {
    error(D_ERROR|D_FATAL, "Failed to open '%s' for writing.\n", filename);
  }

  matrix_fwrite(f, m1, coordinate);
  fclose(f);
}

#endif
