
#ifndef _TYPES_H_
#define _TYPES_H_

#include "mmio.h"

/* Some very basic types we use everywhere, and are too lazy to use
   the full name */
typedef unsigned char uchar;
typedef unsigned int  uint;

/* Define if the data is owned by the structure, or if it is just a
   view of the data.  We may extend this in the future to include
   copy-on-write. */
namespace ownership {
  typedef enum {
    creator,
    viewer
  } type_t;
}

namespace datatype {
  typedef enum {
    unknown,
    vector,
    matrix,
    tensor
  } type_t;
}

void datatype_read_typecode(FILE *file, MM_typecode *type);
datatype::type_t typecode_to_datatype(MM_typecode type);
void datatype_to_typecode(MM_typecode *type, datatype::type_t datatype);

#endif

