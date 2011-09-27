
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "mmio.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

matrix_t*
matrix_read_array(FILE *f)
{
  int      m, n, result;
  uint     i, j;
  matrix_t *mr;
  
  if (0 != (result = mm_read_matrix_array_size(f, &m, &n))) {
    die("Failed to read matrix size (%d).\n", result);
  }
  
  mr = matrix_malloc(m, n);
  
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

  if (0 != (result = mm_read_matrix_coordinate_size(f, &m, &n, &nnz))) {
    die("Failed to read matrix size (%d).\n", result);
  }
  
  mr = matrix_malloc(m, n);
  matrix_clear(mr);
  
  while (nnz--) {
    fscanf(f, "%u %u %lg\n", &i, &j, &d);
    mr->data[i-1][j-1] = d;     /* adjust from 1-based to 0-based */
  }

  return mr;
}

matrix_t*
matrix_fread_data(FILE *file, MM_typecode type)
{
  matrix_t *mr;
  
  debug("matrix_fread_data(file=0x%x, type='%s')\n", 
	file, mm_typecode_to_str(type));
  
  if (!mm_is_matrix(type) || !mm_is_real(type)) {
    die("No support for matrix type: [%s]\n", mm_typecode_to_str(type));
  }
  
  if (mm_is_array(type)) {
    mr = matrix_read_array(file);
  } else {
    mr = matrix_read_coordinate(file);
  }
  
  mr->scheme = scheme::general;
  if (mm_is_symmetric(type)) {
    mr->scheme = scheme::symmetric;
  }
  
  return mr;
}

matrix_t*
matrix_fread(FILE *file)
{
  MM_typecode type;
  
  debug("matrix_fread(0x%x)\n", file);
  
  datatype_read_typecode(file, &type);
  return matrix_fread_data(file, type);
}

matrix_t*
matrix_read(char const *filename)
{
  FILE     *f;
  matrix_t *t;
  
  f = fopen_or_die(filename, "r");
  t = matrix_fread(f);
  fclose(f);

  return t;  
}

