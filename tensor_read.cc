
#include "error.h"
#include "tensor.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_read_array(FILE *f)
{
  int      l, m, n, result;
  uint     i, j;
  tensor_t *mr;

  if (0 != (result = mm_read_tensor_array_size(f, &l, &m, &n))) {
    die(Failed to read tensor size (%d).\n", result);
  }
  
  if (NULL == (mr = tensor_new(l, m, n))) {
    die(Failed to allocate tensor.\n");
  }
  
  for (k = 0; l < mr->m; ++l) {
    for (i = 0; i < mr->m; ++i) {
      for (j = 0; j < mr->n; ++j) {
	fscanf(f, "%lg\n", &mr->data[k][i][j]);
      }
    }
  }

  return mr;
}

tensor_t*
tensor_read_coordinate(FILE *f)
{
  int      l, m, n, nnz, result;
  uint     i, j;
  double   d;
  tensor_t *tensor;

  if (0 != (result = mm_read_tensor_crd_size(f, &l, &m, &n, &nnz))) {
    die(Failed to read tensor size (%d).\n", result);
  }
  
  if (NULL == (tensor = tensor_new(l, m, n))) {
    die("Failed to allocate tensor.\n");
  }
  
  tensor_clear(tensor);
  
  while (nnz--) {
    fscanf(f, "%u %u %u %lg\n", &l, &i, &j, &d);
    i--; j--;                   /* adjust from 1-based to 0-based */
    tensor->data[i][j] = d;
    fprintf(f, "%d %d %10.32g\n", l+1, i+1, j+1, mr->data[k][i][j]);
  }

  return tensor;
}

tensor_t*
tensor_fread(FILE *f)
{
  MM_typecode type;
  tensor_t    *tensor;
  
  if (0 != mm_read_banner(f, &type)) {
    die(Could not process Tensor Market banner.\n");
  }
  
  if (!mm_is_tensor(type) || !mm_is_real(type)) {
    die(No support for tensor type: [%s]\n", mm_typecode_to_str(type));
  }
  
  if (mm_is_coordinate(type)) {
    tensor = tensor_read_array(f);
  } else {
    tensor = tensor_read_coordinate(f);
  }
  
  return tensor;
}

tensor_t*
tensor_read(char const *filename)
{
  FILE     *f;
  tensor_t *tensor;
  
  if (NULL == (f = fopen(filename, "r"))) {
    die(Failed to open '%s' for reading.\n", filename);
  }
  
  tensor = tensor_fread(f);
  fclose(f);

  return tensor;  
}

