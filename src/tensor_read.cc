
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
    tuples[q].index   = q;
    tuples[q].i       = i;
    tuples[q].j       = j;
    tuples[q].k       = k;
    tensor->values[q] = d;
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
  storage->rn    = size;
  storage->cn    = nnz;
  storage->kn    = nnz;
  storage->RO    = MALLOC_N(uint, storage->rn);
  
  for (i = 0; i < storage->rn; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->RO[i] = j;
  }
  
  for (i = 0; i < storage->cn; ++i) {
    if (3 != (result = fscanf(file, "%u %u %lg\n", &j, &k, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->CO[i]    = j;
    storage->KO[i]    = k;
    tensor->values[i] = d;
  }
  
  return tensor;
}

tensor_t*
tensor_fread_compressed_slice(FILE *file)
{
  uint                 i, j;
  int                  l, m, n, nnz, rn, cn, kn;
  int                  result;
  char                 name[20];
  double               d;
  tensor_t             *tensor;
  orientation::type_t  orientation;
  tensor_storage_compressed_t *storage;
  
  debug("tensor_fread_compressed_slice(0x%x)\n", file);
  
  if (0 != (result = mm_read_tensor_compressed_slice_size(file, &l, &m, &n, &nnz, name, &rn, &cn, &kn))) {
    die("Failed to read tensor dimensions (%d).\n", result);
  }
  
  debug("tensor_fread_compressed_slice: l=%d, m=%d, n=%d, name='%s'\n", l, m, n, name);
  
  orientation    = string_to_orientation(name);
  tensor         = tensor_malloc(l, m, n, nnz, strategy::slice, orientation);
  storage        = STORAGE_COMPRESSED(tensor);
  storage->rn    = rn;
  storage->cn    = cn;
  storage->kn    = kn;
  storage->RO    = MALLOC_N(uint, storage->rn);
  
  for (i = 0; i < storage->rn; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->RO[i] = j;
  }
  
  for (i = 0; i < storage->cn; ++i) {
    if (1 != (result = fscanf(file, "%u\n", &j))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->CO[i] = j;
  }
    
  for (i = 0; i < storage->kn; ++i) {
    if (2 != (result = fscanf(file, "%u %lg\n", &j, &d))) {
      die("Failed to process line %d of the input stream (%d).\n", i, result);
    }
    storage->KO[i]    = j;
    tensor->values[i] = d;
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
  storage->rn    = size;
  storage->RO    = MALLOC_N(uint, storage->rn);
  
  for (i = 0; i < size; ++i) {
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
tensor_fread_mmio_data(FILE *file, MM_typecode type)
{
  tensor_t         *tensor;
  strategy::type_t strategy;
  
  debug("tensor_fread_mmio_data(file=0x%x, type='%s')\n", 
	file, mm_typecode_to_str(type));
  
  if (!mm_is_tensor(type)) {
    die("The file does not define a tensor.\n");
  }
  
  strategy = typecode_to_strategy(type);
  tensor   = NULL;
  
  debug("tensor_fread_mmio_data: reading strategy '%s' (%d)\n",
	strategy_to_string(strategy), strategy);
  
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
    tensor = tensor_fread_compressed_slice(file);
    break;
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
tensor_fread_mmio(FILE *file)
{
  MM_typecode type;
  
  debug("tensor_fread_mmio(0x%x)\n", file);
  
  datatype_read_typecode(file, &type);
  return tensor_fread_mmio_data(file, type);
}

tensor_t*
tensor_fread_matlab(FILE *file)
{
  uint                 i, j, k;
  int                  l, m, n, q, nnz;
  int                  result;
  double               d;
  tensor_t             *tensor;
  tensor_storage_coordinate_t *storage;
  coordinate_tuple_t   *tuples;
  
  debug("tensor_fread_matlab(0x%x)\n", file);
  
  /* Read matlab sparse tensor files of the form:
     
     ^X is a sparse tensor of size 10 x 10 x 10 with 96 nonzeros$
     ^	( 1, 1, 7)    0.3796$
     ^	( 1, 2, 5)    0.3191$
     ^	( 1, 3, 2)    0.9861$
	...
    
    Where ^, $ are regular regex anchors (no pun intended).
  */
  
  /* read a line of the form:
     ^X is a sparse tensor of size 10 x 10 x 10 with 96 nonzeros$
  */
  if (4 != (result = fscanf(file, "%*[^1234567890]%d%*[ x]%d%*[ x]%d with %d nonzeros\n", &l, &m, &n, &nnz))) {
    die("tensor_fread_matlab: failed to read tensor dimensions (%d).\n", result);
  }
  
  tensor  = tensor_malloc(l, m, n, nnz, strategy::coordinate);
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  
  for (q = 0; q < nnz; ++q) {
    /* read a line of the form:
       ^  ( 1, 3, 2)    0.9861$
    */
    
    if (4 != (result = fscanf(file, "%*[ (]%u%*[ ,]%u%*[ ,]%u%*[)]%lg\n", &k, &i, &j, &d))) {
      die("tensor_fread_matlab: failed to process line %d of the input stream (%d).\n", q, result);
    }
    tuples[q].index   = q;
    tuples[q].i       = i;
    tuples[q].j       = j;
    tuples[q].k       = k;
    tensor->values[q] = d;
  }
  
  return tensor;
}

file_format::type_t
detect_file_format(FILE *file)
{
  char                c;
  file_format::type_t format;
  
  debug("detect_file_format(0x%x)\n", file);
  
  if (EOF != (c = peek(file))) {
    if ('%' == c) {
      format = file_format::mmio;
    } else {
      format = file_format::matlab;
    }
  }
  
  return format;
}

tensor_t*
tensor_fread_file_format(FILE *file, file_format::type_t format)
{
  tensor_t *tensor;
  
  debug("tensor_fread_file_format(0x%x, %d)\n", file, format);
  
  tensor = NULL;
  
  switch (format) {
  case file_format::mmio:
    tensor = tensor_fread_mmio(file);
    break;
  case file_format::matlab:
    tensor = tensor_fread_matlab(file);
    break;
  default:
    die("tensor_fread_file_format: unknown file type %d.\n", format);
    break;
  }
  
  return tensor;
}

tensor_t*
tensor_read(char const *filename)
{
  FILE                *file;
  tensor_t            *tensor;
  file_format::type_t format;
  
  debug("tensor_read('%s')\n", filename);
  
  file   = fopen_or_die(filename, "r");
  format = detect_file_format(file);
  tensor = tensor_fread_file_format(file, format);
  fclose(file);
  
  return tensor;
}
