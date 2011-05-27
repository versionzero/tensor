
#include "error.h"
#include "file.h"
#include "tensor.h"
#include "memory.h"
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
  coordinate_tuple_t   *tuples;
  
  debug("tensor_fread_coordinate(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_coordinate_size(file, &l, &m, &n, &nnz))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  tensor  = tensor_malloc(l, m, n, nnz, strategy::coordinate);
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  
  for (q = 0; q < nnz; ++q) {
    if (4 != (result = fscanf(file, "%u %u %u %lg\n", &k, &i, &j, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", q, result);
    }
    tensor->values[q] = d;
    tuples[q].i       = i;
    tuples[q].j       = j;
    tuples[q].k       = k;
    tuples[q].index   = q;
  }
  
  return tensor;
}

tensor_t*
tensor_fread_compressed(FILE *file)
{
  uint                 i, j, k;
  int                  l, m, n, nnz, size;
  int                  result;
  char                 name[20];
  double               d;
  tensor_t             *tensor;
  orientation::type_t  orientation;
  storage_compressed_t *storage;
  uint                 *indices;
  
  debug("tensor_fread_compressed(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_compressed_size(file, &l, &m, &n, &nnz, name, &size))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  orientation = string_to_orientation(name);
  tensor      = tensor_malloc(l, m, n, nnz, strategy::compressed, orientation);
  storage     = STORAGE_COMPRESSED(tensor);
  indices     = MALLOC_N(uint, size);
  
  for (i = 0; i < size; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    indices[i] = j;
  }
  
  storage->RO = indices;
  
  for (i = 0; i < nnz; ++i) {
    if (3 != (result = fscanf(file, "%u %u %lg\n", &j, &k, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    tensor->values[i] = d;
    storage->CO[i]    = j;
    storage->KO[i]    = k;
  }
  
  return tensor;
}

tensor_t*
tensor_fread(FILE *file)
{
  MM_typecode      type;
  tensor_t         *tensor;
  int              result;
  strategy::type_t strategy;
  
  debug("tensor_fread(0x%x)\n", file);
  
  if (0 != (result = mm_read_banner(file, &type))) {
    die("Could not process Matrix Market banner: %s.\n", 
	mm_error_to_str(result));
  }
  
  if (!mm_is_tensor(type)) {
    die("The file does not define a tensor.\n");
  }
  
  strategy = typecode_to_strategy(type);
  
  switch (strategy) {
  case strategy::coordinate:
    tensor = tensor_fread_coordinate(file);
    break;
  case strategy::compressed:
    tensor = tensor_fread_compressed(file);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", strategy);
  }
  
  debug("tensor_fread: tensor=0x%x\n", tensor);
  
  return tensor;
}

tensor_t*
tensor_read(char const *filename)
{
  FILE     *file;
  tensor_t *tensor;
  
  debug("tensor_read('%s')\n", filename);
  
  file = fopen_or_die(filename, "r");
  tensor = tensor_fread(file);
  fclose(file);
  
  return tensor;
}
