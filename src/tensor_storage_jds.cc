
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint nnz, index;
} location_t;

static int
compare_location(location_t const *a, location_t const *b)
{
  double c;
  
  c = a->nnz - b->nnz;
  
  if (c < 0) {
    return -1;
  } else if (c > 0) {
    return 1;
  }
  
  return 0;
}

  
#if 0  
  /* for each horizontal slice, sort by row (lateral axis) then col
     (frontal axis), which will give us the JDS matrix for the
     slice */
  offset = 0;
  for (i = 0; i < n; ++i) {
    if (slice_nnz[i] > 0) {
      qsort(dtuples + offset, slice_nnz[i], sizeof(coordinate_tuple_t),
	    (index_compare_t) &index_compare_jki);
      offset += slice_nnz[i];
    }
  }
  
#endif

static void
coordinate_to_jds_compressed_horizontal_columns(tensor_t *destination, tensor_t *source)
{
  uint                        i, j, k, offset, minimum;
  uint                        n, n2, nnz;
  uint                        *permutation;
  uint                        *lengths, *sizes;
  bool                        *used;
  uint                        *jdptr, *colind, *rowind;
  location_t                  *columns;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *src, *dest;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_COMPRESSED(destination);
  
  debug("coordinate_to_jds_compressed_horizontal_columns(destination=0x%x, source=0x%x)\n", destination, source);
  
  base         = STORAGE_BASE(destination);
  nnz          = source->nnz;
  n            = source->n;
  n2           = n*n;
  src          = source->values;
  dest         = destination->values;
  tuples       = s->tuples;
  columns      = MALLOC_N(location_t, n);
  lengths      = MALLOC_N(uint, n);
  sizes        = MALLOC_N(uint, n);
  used         = MALLOC_N(bool, nnz);
  colind       = d->CO;
  rowind       = d->RO;
  permutation  = d->KO;
  jdptr        = d->TO;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t),
	(index_compare_t) &index_compare_ijk);
  
  for (i = 0; i < n; ++i) {
    sizes[i] = 0;
  }
  for (i = 0; i < n; ++i) {
    columns[i].nnz   = 0;
    columns[i].index = 0;
  }
  for (i = 0; i < nnz; ++i) {
    used[i] = false;
  }
  
  /* count the number of non-zeros in the columns of each lateral
     slice */
  for (i = 0; i < nnz; ++i) {
    
    message("%d %d %d\n", tuples[i].i, tuples[i].j, tuples[i].k);
    
    k                = tuples[i].k;
    columns[k].index = k;
    columns[k].nnz++;
  }
  
  for (i = 0; i < n; ++i) {
    DEBUG("col(%d) has %d nnz\n", i, columns[i].nnz);
  }
  
  /* find the shortest column of nonzeros.  this will determine the
     largest possible set of dense tubes we can create. */
  minimum = nnz;
  for (i = 0; i < n; ++i) {
    if (minimum > columns[i].nnz) {
      minimum = columns[i].nnz;
    }
  }
  
  /* create our dense vectors by packing all non-zeros in a column
     until we reach capacity on each column (as determined above) */
  for (i = 0; i < nnz; ++i) {
    k = tuples[i].k;
    if (!used[i] && sizes[k] < minimum) {
      DEBUG("storing> %d %d %d\t%f\n", tuples[i].i, tuples[i].j, tuples[i].k, src[tuples[i].index]);
      used[i]        = true;
      offset         = (minimum - 1) * sizes[k] + k;
      dest[offset]   = src[tuples[i].index];
      colind[offset] = tuples[i].j;
      rowind[offset] = tuples[i].i;
      sizes[k]++;
    }
  }
  
  /* Sort the remaining unused non-zeros from the unfolded tensor by
     nnz per column and set up permutation array. */
  for (i = 0; i < n; ++i) {
    columns[i].nnz -= minimum;
  }
  
  qsort(columns, n, sizeof(location_t), 
	(index_compare_t) &compare_location);
  
  for (i = 0; i < n; ++i) {
    j              = columns[i].index;
    permutation[i] = j;
    lengths[i]     = columns[i].nnz;
  }
  
  /* configure jdptr */
  jdptr[0] = (minimum) * n;
  for(i = 1; i < n + 1; ++i) {
    jdptr[i] = jdptr[i-1] + lengths[i-1];
  }
  
  /* with the remaining non-zeros, create dense but incomplete
     tubes */
  for (i = 0; i < n; ++i) {
    sizes[i] = 0;
  }
  for (i = 0; i < nnz; ++i) {
    k = tuples[i].k;
    if (!used[i]) {
      used[i]        = true;
      offset         = jdptr[k] + sizes[k];
      message("storing2> %d %d %d\t%f\t@\t%d\n", tuples[i].i, tuples[i].j, tuples[i].k, src[tuples[i].index], offset);
      dest[offset]   = src[tuples[i].index];
      colind[offset] = tuples[i].j;
      rowind[offset] = tuples[i].i;
      sizes[k]++;
    }
  }

}

void
tensor_storage_convert_from_coordinate_to_jds(tensor_t *destination, tensor_t *source)
{
  debug("tensor_storage_convert_from_coordinate_to_jds(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::horizontal:
    coordinate_to_jds_compressed_horizontal_columns(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

tensor_storage_compressed_t*
tensor_storage_malloc_jds(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  
  superfluous("tensor_storage_malloc_jds(tensor=0x%x)\n", tensor);
  
  storage	  = MALLOC(tensor_storage_compressed_t);
  storage->rn	  = tensor->nnz;
  storage->cn	  = tensor->nnz;
  storage->kn	  = tensor->n;
  storage->tn	  = tensor->n + 1;
  storage->RO	  = MALLOC_N(uint, storage->rn);
  storage->CO	  = MALLOC_N(uint, storage->cn);
  storage->KO	  = MALLOC_N(uint, storage->kn);
  storage->TO	  = MALLOC_N(uint, storage->tn);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = NULL;
  
  superfluous("tensor_storage_malloc_jds: base=0x%x\n", base);
  superfluous("tensor_storage_malloc_jds: base->callbacks=0x%x\n", base->callbacks);
  superfluous("tensor_storage_malloc_jds: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_jds: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_jds: storage->TO=0x%x\n", storage->TO);
  superfluous("tensor_storage_malloc_jds: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_jds: storage=0x%x\n", storage);
  
  return storage;
}
