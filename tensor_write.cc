
#include "error.h"
#include "file.h"
#include "math.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
tensor_initialize_typecode(MM_typecode *type, strategy::type_t strategy)
{
  mm_initialize_typecode(type);
  datatype_to_typecode(type, datatype::tensor);
  strategy_to_typecode(type, strategy);
  mm_set_real(type);
}

void
tensor_fwrite_coordinate(FILE *file, tensor_t const *tensor)
{
  uint                 i, nnz;
  int                  result;
  MM_typecode          type;
  storage_coordinate_t *storage;
  coordinate_tuple_t   *tuples;
  
  debug("tensor_write_coordinate(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  tensor_initialize_typecode(&type, strategy::coordinate);
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Tensor Market banner (%d).\n", result);
  }
  
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  nnz     = 0;
  
  for (i = 0; i < tensor->nnz; ++i) {
    if (!might_as_well_be_zero(tensor->values[i])) {
      nnz++;
    }
  }
  
  debug("tensor_write_coordinate: non-zero values: implied=%d, actual=%d.\n", tensor->nnz, nnz);
  debug("tensor_write_coordinate: l=%d, m=%d, n=%d.\n", tensor->l, tensor->m, tensor->n);
  
  if (0 != (result = mm_write_tensor_coordinate_size(file, tensor->l, tensor->m, tensor->n, nnz))) {
    die("Failed to write coordinate tensor of size %d (%d).\n", nnz, result);
  }
  
  for (i = 0; i < nnz; ++i) {
    if (!might_as_well_be_zero(tensor->values[i])) {
      fprintf(file, "%d %d %d %10.32g\n", 
	      tuples[i].k, tuples[i].i, tuples[i].j,
	      tensor->values[tuples[i].index]);
    }
  }
}

void
tensor_fwrite_compressed(FILE *file, tensor_t const *tensor)
{
  uint                 l, m, n;
  int                  i, nnz, size, result;
  MM_typecode          type;
  storage_compressed_t *storage;
  char const           *name;
  
  debug("tensor_write_compressed(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  tensor_initialize_typecode(&type, strategy::compressed);
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Tensor Market banner (%d).\n", result);
  }
  
  storage = STORAGE_COMPRESSED(tensor);
  l       = tensor->l;
  m       = tensor->m;
  n       = tensor->n;
  nnz     = tensor->nnz;
  size    = storage->size;
  name    = orientation_to_string(tensor->orientation);
  
  debug("tensor_write_compressed: l=%d, m=%d, n=%d, nnz=%d, orientation='%s', size=%d.\n", 
	l, m, n, nnz, name, size);
  
  /* Fixing zeros here may be tricky, so I'll leave it until it is
     required.  The problem with zeros here is that if we find one, we
     will need to update the offset indices in KO.  Addmitedly, this
     should not be too difficult, but it may take some time to ensure
     the re-indexing is correct. */
  
  if (0 != (result = mm_write_tensor_compressed_size(file, l, m, n, nnz, name, size-1))) {
    die("Failed to write compressed tensor of size %d (%d).\n", nnz, result);
  }
  
  /* we don't write out the 0th entry-- it is a constant, namely 0 */
  for (i = 1; i < size; ++i) {
    fprintf(file, "%d\n", storage->RO[i]);
  }
  
  for (i = 0; i < nnz; ++i) {
    fprintf(file, "%d %d %10.32g\n",
	    storage->CO[i], storage->KO[i],
	    tensor->values[i]);
  }
}

void
tensor_fwrite_extended_compressed(FILE *file, tensor_t const *tensor, strategy::type_t strategy)
{
  uint               l, m, n;
  int                i, nnz, size, result;
  MM_typecode        type;
  storage_extended_t *storage;
  char const         *name;
  
  debug("tensor_fwrite_extended_compressed(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  tensor_initialize_typecode(&type, strategy);
  
  debug("tensor_fwrite_extended_compressed: strategy='%s'.\n", 
	strategy_to_string(strategy));
  
  if (0 != (result = mm_write_banner(file, type))) {
    die("Could not write Tensor Market banner (%d).\n", result);
  }
  
  storage = STORAGE_EXTENDED(tensor);
  l       = tensor->l;
  m       = tensor->m;
  n       = tensor->n;
  nnz     = tensor->nnz;
  size    = storage->size;
  name    = orientation_to_string(tensor->orientation);
  
  debug("tensor_fwrite_extended_compressed: l=%d, m=%d, n=%d, nnz=%d, orientation='%s', size=%d.\n", 
	l, m, n, nnz, name, size);
  
  /* Fixing zeros here may be tricky, so I'll leave it until it is
     required.  The problem with zeros here is that if we find one, we
     will need to update the offset indices in KO.  Addmitedly, this
     should not be too difficult, but it may take some time to ensure
     the re-indexing is correct. */
  
  if (0 != (result = mm_write_tensor_compressed_size(file, l, m, n, nnz, name, size-1))) {
    die("Failed to write compressed tensor of size %d (%d).\n", nnz, result);
  }
  
  /* we don't write out the 0th entry-- it is a constant, namely 0 */
  for (i = 1; i < size; ++i) {
    fprintf(file, "%d\n", storage->RO[i]);
  }
  
  for (i = 0; i < nnz; ++i) {
    fprintf(file, "%d %10.32g\n",
	    storage->CK[i], tensor->values[i]);
  }
}

void
tensor_fwrite(FILE *file, tensor_t const *tensor)
{
  debug("tensor_write(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    tensor_fwrite_coordinate(file, tensor);
    break;
  case strategy::compressed:
    tensor_fwrite_compressed(file, tensor);
    break;
  case strategy::ekmr:
  case strategy::pkmr:
  case strategy::zzpkmr:
    tensor_fwrite_extended_compressed(file, tensor, tensor->strategy);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", 
	strategy_to_string(tensor->strategy));
  }
}

void
tensor_write(char const *filename, tensor_t const *tensor)
{
  FILE *file;
  
  debug("tensor_write(filename='%s', tensor=0x%x)\n", filename, tensor);
  
  file = fopen_or_die(filename, "w+");
  tensor_fwrite(file, tensor);
  fclose(file);
}

