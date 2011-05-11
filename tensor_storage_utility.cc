
#include "storage.h"
#include "tensor.h"

char const*
tensor_storage_name(storage_strategy_t strategy)
{
  static char const *name[] = { 
    "Coordinate", 
    "EKMR" 
  };
  return name[strategy];
}

