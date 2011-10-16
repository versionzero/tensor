
#include "error.h"
#include "file.h"
#include "vector.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

vector_t*
vector_read_array(FILE *f)
{
  int      n, result;
  uint     i;
  vector_t *v;

  if (0 != (result = mm_read_vector_array_size(f, &n))) {
    die("Failed to read vector size (%d).\n", result);
  }
  
  v = vector_malloc(n);

  for (i = 0; i < v->n; ++i) {
    fscanf(f, "%u\n", &v->data[i]);
  }

  return v;
}

vector_t*
vector_fread_data(FILE *file, MM_typecode type)
{ 
  debug("vector_fread_data(file=0x%x, type='%s')\n", 
	file, mm_typecode_to_str(type));
  
  if (!mm_is_vector(type) || !mm_is_integer(type)) {
    die("No support for vector type: [%s]\n", mm_typecode_to_str(type));
  }
  
  return vector_read_array(file);
}

vector_t*
vector_fread(FILE *file)
{
  MM_typecode type;
  
  debug("vector_fread(0x%x)\n", file);
  
  datatype_read_typecode(file, &type);
  return vector_fread_data(file, type);
}

vector_t*
vector_read(char const *filename)
{
  FILE     *f;
  vector_t *t;
  
  f = fopen_or_die(filename, "r");
  t = vector_fread(f);
  fclose(f);

  return t;  
}

