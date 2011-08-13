
#include "error.h"
#include "file.h"
#include "tensor.h"
#include "memory.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

tensor_t*
tensor_fread_array(FILE *file)
{
  int                  i, j, k, v;
  int                  l, m, n, nnz;
  int                  result;
  double               d;
  tensor_t             *tensor;
  tensor_storage_coordinate_t *storage;
  coordinate_tuple_t   *tuples;
  
  debug("tensor_fread_array(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_array_size(file, &l, &m, &n))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  debug("tensor_fread_array: l=%d, m=%d, n=%d\n", l, m, n);
  
  nnz     = l*m*n;
  tensor  = tensor_malloc(l, m, n, nnz, strategy::coordinate);
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  v       = 0;
  
  for (k = 0; k < l; ++k) {
    for (i = 0; i < m; ++i) {
      for (j = 0; j < n; ++j) {
	if (1 != (result = fscanf(file, "%lg\n", &d))) {
	  die("Failed to process line %d of the input stream (%d).\n", v, result);
	}
	tensor->values[v] = d;
	tuples[v].i       = i;
	tuples[v].j       = j;
	tuples[v].k       = k;
	tuples[v].index   = v;
	v++;
      }
    }
  }
  
  return tensor;
}

tensor_t*
tensor_fread_coordinate(FILE *file)
{
  uint                 i, j, k;
  int                  l, m, n, q, nnz;
  int                  result;
  double               d;
  tensor_t             *tensor;
  tensor_storage_coordinate_t *storage;
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
  tensor_storage_compressed_t *storage;
  
  debug("tensor_fread_compressed(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_compressed_size(file, &l, &m, &n, &nnz, name, &size))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  orientation    = string_to_orientation(name);
  tensor         = tensor_malloc(l, m, n, nnz, strategy::compressed, orientation);
  storage        = STORAGE_COMPRESSED(tensor);
  storage->rn    = size+1;
  storage->cn    = nnz;
  storage->kn    = nnz;
  storage->RO    = MALLOC_N(uint, storage->rn);
  storage->RO[0] = 0;
  
  for (i = 1; i < storage->rn; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->RO[i] = j;
  }
  
  for (i = 0; i < storage->cn; ++i) {
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
tensor_fread_extended_compressed(FILE *file, strategy::type_t strategy)
{
  uint                      j;
  int                       i, l, m, n, nnz, size;
  int                       result;
  char                      name[20];
  double                    d;
  tensor_t                  *tensor;
  orientation::type_t       orientation;
  tensor_storage_extended_t *storage;
  
  debug("tensor_fread_compressed(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_compressed_size(file, &l, &m, &n, &nnz, name, &size))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  orientation    = string_to_orientation(name);
  tensor         = tensor_malloc(l, m, n, nnz, strategy, orientation);
  storage        = STORAGE_EXTENDED(tensor);
  storage->size  = size+1;
  storage->RO    = MALLOC_N(uint, storage->size);
  storage->RO[0] = 0;
  
  for (i = 1; i <= size; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->RO[i] = j;
  }
  
  for (i = 0; i < nnz; ++i) {
    if (2 != (result = fscanf(file, "%u %lg\n", &j, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    tensor->values[i] = d;
    storage->CK[i]    = j;
  }
  
  return tensor;
}

tensor_t*
tensor_fread_data(FILE *file, MM_typecode type)
{
  tensor_t         *tensor;
  strategy::type_t strategy;
  
  debug("tensor_fread_data(file=0x%x, type='%s')\n", 
	file, mm_typecode_to_str(type));
  
  if (!mm_is_tensor(type)) {
    die("The file does not define a tensor.\n");
  }
  
  strategy = typecode_to_strategy(type);
  tensor   = NULL;
  
  switch (strategy) {
  case strategy::array:
    tensor = tensor_fread_array(file);
    break;
  case strategy::coordinate:
    tensor = tensor_fread_coordinate(file);
    break;
  case strategy::compressed:
    tensor = tensor_fread_compressed(file);
    break;
  case strategy::slice:
  case strategy::ekmr:
  case strategy::zzekmr:
    tensor = tensor_fread_extended_compressed(file, strategy);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", strategy);
  }
  
  debug("tensor_fread: tensor=0x%x\n", tensor);
  
  return tensor;
}

tensor_t*
tensor_fread(FILE *file)
{
  MM_typecode type;
  
  debug("tensor_fread(0x%x)\n", file);
  
  datatype_read_typecode(file, &type);
  return tensor_fread_data(file, type);
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
