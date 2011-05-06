
#include "error.h"
#include "file.h"
#include "mmio.h"
#include "tensor.h"

#include <stdio.h>
#include <stdlib.h>

void
tensor_initialize_type(MM_typecode *type)
{
  mm_initialize_typecode(type);
  mm_set_tensor(type);
  mm_set_real(type);
}

void
tensor_write_array(FILE *f, tensor_t const *t1)
{
  uint        i, j, k;
  int         result;
  MM_typecode type;
  
  tensor_initialize_type(&type);
  mm_set_array(&type);
  
  if (0 != (result = mm_write_banner(f, type))) {
    die("Could not write Matrix Market banner (%d).\n", result);
  }
  
  if (0 != (result = mm_write_tensor_array_size(f, t1->l, t1->m, t1->n))) {
    die("Failed to write tensor array size (%d).\n", result);
  }

  for (k = 0; k < t1->l; ++k) {
    for (i = 0; i < t1->m; ++i) {
      for (j = 0; j < t1->n; ++j) {
	fprintf(f, "%10.32g\n", t1->data[k][i][j]);
      }
    }
  }
}

void
tensor_write_coordinate(FILE *f, tensor_t const *t1)
{
  uint        i, j, k;
  int         nnz, result;
  MM_typecode type;
  
  tensor_initialize_type(&type);
  mm_set_coordinate(&type);
  
  if (0 != (result = mm_write_banner(f, type))) {
    die("Could not write Tensor Market banner (%d).\n", result);
  }
  
  nnz = 0;
  for (k = 0; k < t1->l; ++k) {
    for (i = 0; i < t1->m; ++i) {
      for (j = 0; j < t1->n; ++j) {
	if (0.0 != t1->data[k][i][j]) {
	  nnz++;
	}
      }
    }
  }
  
  if (0 != (result = mm_write_tensor_coordinate_size(f, t1->m, t1->n, nnz))) {
    die("Failed to write tensor coordinate size %d (%d).\n", nnz, result);
  }

  for (k = 0; k < t1->l; ++k) {
    for (i = 0; i < t1->m; ++i) {
      for (j = 0; j < t1->n; ++j) {
	if (0.0 != t1->data[k][i][j]) {
	  fprintf(f, "%d %d %d %10.32g\n", k+1, i+1, j+1, t1->data[i][j]);
	}
      }
    }
  }
}

void
tensor_fwrite(FILE *f, tensor_t const *t1, bool coordinate)
{
  if (coordinate) {
    tensor_write_coordinate(f, t1);
  } else {
    tensor_write_array(f, t1);
  }
}

void
tensor_write(char const *filename, tensor_t const *t1, bool coordinate)
{
  FILE *f;

  f = open_or_die(filename, "w+");
  tensor_fwrite(f, t1, coordinate);
  fclose(f);
}

