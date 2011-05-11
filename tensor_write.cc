
#include "error.h"
#include "file.h"
#include "math.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

bool
might_as_well_be_zero(double d)
{
  return (fabs(d) < EPSILON);
}

void
tensor_initialize_type(MM_typecode *type)
{
  mm_initialize_typecode(type);
  mm_set_tensor(type);
  mm_set_real(type);
}

void
tensor_write_coordinate(FILE *file, tensor_t const *tensor)
{
  uint                 i;
  int                  nnz, result;
  MM_typecode          type;
  storage_coordinate_t *storage;
  
  information("tensor_write_coordinate(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  tensor_initialize_type(&type);
  mm_set_coordinate(&type);
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Tensor Market banner (%d).\n", result);
  }
  
  storage = STORAGE_COORIDINATE(tensor);
  nnz     = 0;
  
  information("tensor_write_coordinate: counting non-zero values...\n");
  
  for (i = 0; i < storage->nnz; ++i) {
    if (!might_as_well_be_zero(storage->values[i])) {
      nnz++;
    }
  }
  
  information("tensor_write_coordinate: implied=%d, actual=%d.\n", storage->nnz, nnz);
  information("tensor_write_coordinate: l=%d, m=%d, n=%d.\n", tensor->l, tensor->m, tensor->n);
  
  if (0 != (result = mm_write_tensor_coordinate_size(file, tensor->l, tensor->m, tensor->n, nnz))) {
    die("Failed to write tensor coordinate size %d (%d).\n", nnz, result);
  }
  
  for (i = 0; i < nnz; ++i) {
    if (!might_as_well_be_zero(storage->values[i])) {
      fprintf(file, "%d %d %d %10.32g\n", 
	      storage->K[i]+1, storage->I[i]+1, storage->J[i]+1, 
	      storage->values[i]);
    }
  }
}

void
tensor_fwrite(FILE *file, tensor_t const *tensor)
{
  information("tensor_write(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  tensor_write_coordinate(file, tensor);
}

void
tensor_write(char const *filename, tensor_t const *tensor)
{
  FILE *file;
  
  information("tensor_write(filename='%s', tensor=0x%x)\n", filename, tensor);
  
  file = fopen_or_die(filename, "w+");
  tensor_fwrite(file, tensor);
  fclose(file);
}

