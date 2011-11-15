
#include "data.h"
#include "error.h"
#include "utility.h"
#include <string.h>

static char const *map_data_partition_to_string[] = { 
  "unknown",
  "fiber",
  "slice"
};

static char const *map_data_partition_to_description[] = { 
  "unknown",
  "fiber per data",
  "slice per data"
};

char const*
data_partition_to_string(data::partition::type_t partition)
{
  return map_data_partition_to_string[partition];
}

data::partition::type_t
string_to_data_partition(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_data_partition_to_string); ++i) {
    if (0 == strcmp(name, map_data_partition_to_string[i])) {
      return (data::partition::type_t) i;
    }
  }
  
  return data::partition::unknown;
}

void
print_data_partitions(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_data_partition_to_string); ++i) {
    message(format, map_data_partition_to_string[i]);
  }
}

void
print_data_partitions_with_descriptions(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_data_partition_to_string); ++i) {
    message(format, map_data_partition_to_string[i], map_data_partition_to_description[i]);
  }
}
