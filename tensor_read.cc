
#include "error.h"
#include "file.h"
#include "tensor.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_fread_coordinate(FILE *file)
{
  uint                 i, j, k, q;
  int                  l, m, n, nnz;
  int                  result;
  double               d;
  tensor_t             *tensor;
  storage_coordinate_t *storage;
  
  information("tensor_fread_coordinate(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_coordinate_size(file, &l, &m, &n, &nnz))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  tensor  = tensor_new(l, m, n, nnz, coordinate);
  storage = STORAGE_COORIDINATE(tensor);
  
  for (q = 0; q < nnz; ++q) {
    if (4 != (result = fscanf(file, "%u %u %u %lg\n", &k, &i, &j, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", q, result);
    }
    storage->values[q] = d;
    storage->I[q]      = --i;   /* adjust from 1-based to 0-based */
    storage->J[q]      = --j;
    storage->K[q]      = --k;
  }
  
  return tensor;
}

tensor_t*
tensor_fread(FILE *file)
{
  MM_typecode type;
  tensor_t    *tensor;
  int         result;
  
  information("tensor_fread(0x%x)\n", file);
  
  if (0 != (result = mm_read_banner(file, &type))) {
    die("Could not process Matrix Market banner: %s.\n", 
	mm_error_to_str(result));
  }
  
  if (!mm_is_tensor(type)) {
    die("The file '%s' does not define a tensor.\n");
  }
  
  tensor = tensor_fread_coordinate(file);
  fclose(file);
  
  information("tensor_fread: tensor=0x%x\n", tensor);
  
  return tensor;
}

tensor_t*
tensor_read(char const *filename)
{
  FILE     *file;
  tensor_t *tensor;
  
  information("tensor_read('%s')\n", filename);
  
  file = fopen_or_die(filename, "r");
  tensor = tensor_fread(file);
  fclose(file);
  
  return tensor;
}
