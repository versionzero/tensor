
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "types.h"
#include <stdio.h>

typedef struct {
  uint              n;
  ownership::type_t owner;
  double            *data;
} vector_t;

vector_t* vector_malloc(uint n, ownership::type_t owner = ownership::creator);
void vector_free(vector_t *v);
void vector_clear(vector_t *v);

vector_t* vector_read(char const *filename);
vector_t* vector_fread(FILE *stream);
vector_t* vector_fread_data(FILE *file, MM_typecode type);
void vector_write(char const *filename, vector_t const *v);
void vector_fwrite(FILE *stream, vector_t const *v);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
