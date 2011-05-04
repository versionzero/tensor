
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

matrix_t*
matrix_read_array(FILE *f)
{
  int      m, n, result;
  uint     i, j;
  matrix_t *mr;

  if (0 != (result = mm_read_mtx_array_size(f, &m, &n))) {
    error(D_ERROR|D_FATAL, 
	  "Failed to read matrix size (%d).\n", 
	  result);
  }
  
  if (NULL == (mr = matrix_new(m, n))) {
    error(D_ERROR|D_FATAL, "Failed to allocate matrix.\n");
  }
  
  for (i = 0; i < mr->m; ++i) {
    for (j = 0; j < mr->n; ++j) {
      fscanf(f, "%lg\n", &mr->data[i][j]);
    }
  }

  return mr;
}

matrix_t*
matrix_read_coordinate(FILE *f)
{
  int      m, n, nnz, result;
  uint     i, j;
  double   d;
  matrix_t *mr;

  if (0 != (result = mm_read_mtx_crd_size(f, &m, &n, &nnz))) {
    error(D_ERROR|D_FATAL, 
	  "Failed to read matrix size (%d).\n", 
	  result);
  }
  
  if (NULL == (mr = matrix_new(m, n))) {
    error(D_ERROR|D_FATAL, "Failed to allocate matrix.\n");
  }
  
  matrix_clear(mr);
  
  while (nnz--) {
    fscanf(f, "%u %u %lg\n", &i, &j, &d);
    i--; j--;                   /* adjust from 1-based to 0-based */
    mr->data[i][j] = d;
    fprintf(f, "%d %d %10.32g\n", i+1, j+1, mr->data[i][j]);
  }

  return mr;
}

matrix_t*
matrix_fread(FILE *f)
{
  MM_typecode type;
  matrix_t    *mr;
  
  if (0 != mm_read_banner(f, &type)) {
    error(D_ERROR|D_FATAL, 
	  "Could not process Matrix Market banner.\n");
  }
  
  if (!mm_is_matrix(type) || !mm_is_real(type)) {
    error(D_ERROR|D_FATAL, 
	  "No support for matrix type: [%s]\n", 
	  mm_typecode_to_str(type));
  }
  
  if (mm_is_array(type)) {
    mr = matrix_read_array(f);
  } else {
    mr = matrix_read_coordinate(f);
  }
  
  return mr;
}

matrix_t*
matrix_read(char const *filename)
{
  FILE     *f;
  matrix_t *t;
  
  if (NULL == (f = fopen(filename, "r"))) {
    error(D_ERROR|D_FATAL, "Failed to open '%s' for reading.\n", filename);
  }
  
  t = matrix_fread(f);
  fclose(f);

  return t;  
}

