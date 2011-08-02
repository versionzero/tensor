
#include "error.h"
#include "mmio.h"
#include "types.h"

static char const *map_ownership_to_string[] = {
  "creator",
  "viewer"
};

char const*
ownership_to_string(ownership::type_t strategy)
{
  return map_ownership_to_string[strategy];
}

void
datatype_read_typecode(FILE *file, MM_typecode *type)
{
  int result;
  
  debug("datatype_read_typecode(file=0x%x)\n");
  
  if (0 != (result = mm_read_banner(file, type))) {
    die("Could not process Matrix Market banner: %s.\n", 
	mm_error_to_str(result));
  }
}

datatype::type_t
typecode_to_datatype(MM_typecode type)
{
  if (mm_is_vector(type)) {
    return datatype::vector;
  } else if (mm_is_matrix(type)) {
    return datatype::matrix;
  } else if (mm_is_tensor(type)) {
    return datatype::tensor;
  }
  
  return datatype::unknown;
}

void
datatype_to_typecode(MM_typecode *type, datatype::type_t datatype)
{
  switch (datatype) {
  case datatype::vector:
    mm_set_vector(type);
    break;
  case datatype::matrix:
    mm_set_matrix(type);
    break;
  case datatype::tensor:
    mm_set_tensor(type);
    break;
  default:
    die("Datatype '%d' not a supported Matrix Maket type code.\n", datatype);
  }
}
