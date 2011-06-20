
#include "error.h"
#include "file.h"
#include "vector.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

void
vector_initialize_type(MM_typecode *type)
{
  mm_initialize_typecode(type);
  mm_set_vector(type);
  mm_set_real(type);
}

void
vector_write_array(FILE *f, vector_t const *v)
{
  uint        i;
  int         result;
  MM_typecode type;
  
  vector_initialize_type(&type);
  mm_set_array(&type);
  
  if (0 != (result = mm_write_banner(f, type))) {
    die("Could not write Vector Market banner (%d).\n", result);
  }
  
  if (0 != (result = mm_write_vector_array_size(f, v->n))) {
    die("Failed to write vector array size (%d).\n", result);
  }

  for (i = 0; i < v->n; ++i) {
    fprintf(f, "%10.32g\n", v->data[i]);
  }
}


void
vector_fwrite(FILE *f, vector_t const *v, bool coordinate)
{
  vector_write_array(f, v);
}

void
vector_write(char const *filename, vector_t const *v, bool coordinate)
{
  FILE *f;

  f = fopen_or_die(filename, "w+");
  vector_fwrite(f, v, coordinate);
  fclose(f);
}

