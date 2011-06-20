
#include "tool.h"
#include "string.h"
#include "utility.h"
#include <math.h>

static char const *map_tool_to_string[] = { 
  "unknown",
  "convert", 
  "effectuate"
};

char const* string_from_tool(tool::type_t tool)
{
  return map_tool_to_string[tool];
}

tool::type_t
tool_from_string(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_tool_to_string); ++i) {
    if (0 == strcmp(name, map_tool_to_string[i])) {
      return (tool::type_t) i;
    }
  }
  
  return tool::unknown;
}
