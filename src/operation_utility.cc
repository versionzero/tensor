
#include "error.h"
#include "operation.h"
#include "tensor.h"
#include "utility.h"
#include <string.h>

static char const *map_operations_to_string[] = { 
  "unknown",
  "n-mode"
};

static char const *map_operations_to_description[] = { 
  "unknown",
  "dense tensor \\times vector product"
};

char const*
operation_to_string(operation::type_t operation)
{
  return map_operations_to_string[operation];
}

char const*
operation_to_description_string(operation::type_t operation)
{
  return map_operations_to_description[operation];
}

operation::type_t
string_to_operation(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_operations_to_string); ++i) {
    if (0 == strcmp(name, map_operations_to_string[i])) {
      return (operation::type_t) i;
    }
  }
  
  return operation::unknown;
}

void
print_operations(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_operations_to_string); ++i) {
    message(format, map_operations_to_string[i]);
  }
}

void
print_operations_with_descriptions(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_operations_to_string); ++i) {
    message(format, map_operations_to_string[i], map_operations_to_description[i]);
  }
}

static char const *map_associations_to_string[] = { 
  "unknown",
  "left",
  "right"
};

char const*
association_to_string(association::type_t association)
{
  return map_associations_to_string[association];
}

association::type_t
string_to_association(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_associations_to_string); ++i) {
    if (0 == strcmp(name, map_associations_to_string[i])) {
      return (association::type_t) i;
    }
  }
  
  return association::unknown;
}
