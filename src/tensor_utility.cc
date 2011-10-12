
#include "error.h"
#include "tensor.h"
#include "utility.h"
#include <string.h>

static char const *map_permutation_heuristics_to_string[] = {
  "none",
  "naive-greedy"
};

static char const *map_permutation_heuristics_to_description[] = { 
  "none",
  "re-order tensor layout based on intra-slice proximity"
};

char const*
permutation_heuristic_to_string(permutation_heuristic::type_t heuristic)
{
  return map_permutation_heuristics_to_string[heuristic];
}

permutation_heuristic::type_t
string_to_permutation_heuristic(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_permutation_heuristics_to_string); ++i) {
    if (0 == strcmp(name, map_permutation_heuristics_to_string[i])) {
      return (permutation_heuristic::type_t) i;
    }
  }
  
  return permutation_heuristic::none;
}

void
print_permutation_heuristics(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_permutation_heuristics_to_string); ++i) {
    message(format, map_permutation_heuristics_to_string[i]);
  }
}

void
print_permutation_heuristics_with_descriptions(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_permutation_heuristics_to_string); ++i) {
    message(format, map_permutation_heuristics_to_string[i], 
	    map_permutation_heuristics_to_description[i]);
  }
}

static char const *map_strategy_to_string[] = {
  "unknown",
  "array",
  "coordinate",
  "compressed",
  "gundersen",
  "slice",
  "ekmr",
  "zzekmr"
};

strategy::type_t
string_to_strategy(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_strategy_to_string); ++i) {
    if (0 == strcmp(name, map_strategy_to_string[i])) {
      return (strategy::type_t) i;
    }
  }
  
  return strategy::unknown;
}

char const*
strategy_to_string(strategy::type_t strategy)
{
  return map_strategy_to_string[strategy];
}

void
print_strategies(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_strategy_to_string); ++i) {
    message(format, map_strategy_to_string[i]);
  }
}


static char const *map_orientation_to_latex_macro[] = { 
  "unknown",
  "crs", 
  "ccs",
  "cts",
  "clss",
  "chss",
  "cfss"
};

char const*
orientation_to_latex_macro(orientation::type_t orientation)
{
  return map_orientation_to_latex_macro[orientation];
}

static char const *map_orientation_to_string[] = { 
  "unknown",
  "row", 
  "column",
  "tube",
  "lateral",
  "horizontal",
  "frontal"
};

char const*
orientation_to_string(orientation::type_t orientation)
{
  return map_orientation_to_string[orientation];
}

orientation::type_t
string_to_orientation(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_orientation_to_string); ++i) {
    if (0 == strcmp(name, map_orientation_to_string[i])) {
      return (orientation::type_t) i;
    }
  }
  
  return orientation::unknown;
}

void
print_orientations(char const *format)
{
  uint i;
  
  for (i = 1; i < COUNT_OF(map_orientation_to_string); ++i) {
    message(format, map_orientation_to_string[i]);
  }
}

strategy::type_t
typecode_to_strategy(MM_typecode type)
{
  if (mm_is_array(type)) {
    return strategy::array;
  } else if (mm_is_coordinate(type)) {
    return strategy::coordinate;
  } else if (mm_is_compressed(type)) {
    return strategy::compressed;
  } else if (mm_is_gundersen(type)) {
    return strategy::gundersen;
  } else if (mm_is_slice(type)) {
    return strategy::slice;
  } else if (mm_is_ekmr(type)) {
    return strategy::ekmr;
  } else if (mm_is_zzekmr(type)) {
    return strategy::zzekmr;
  }
  
  return strategy::unknown;
}

void
strategy_to_typecode(MM_typecode *type, strategy::type_t strategy)
{
  switch (strategy) {
  case strategy::array:
    mm_set_array(type);
    break;
  case strategy::coordinate:
    mm_set_coordinate(type);
    break;
  case strategy::compressed:
    mm_set_compressed(type);
    break;
  case strategy::slice:
    mm_set_slice(type);
    break;
  case strategy::ekmr:
    mm_set_ekmr(type);
    break;
  case strategy::zzekmr:
    mm_set_zzekmr(type);
    break;
  default:
    die("Storage strategy '%d' not supported for Matrix Maket type code.\n", strategy);
  }
}
