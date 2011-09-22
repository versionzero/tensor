
#include "error.h"
#include "tool.h"
#include "string.h"
#include "utility.h"
#include <math.h>

extern uint              cache_size;
extern uint              cache_line_size;
extern bool              emit_latex;
extern uint              iterations;
extern char              *tool_name;
extern tool::type_t      tool_type;
extern bool              simulate;
extern bool              tracing;
extern bool              verbose;
extern verbosity::type_t noisiness;
extern bool              write_results;

#define TENSOR_DESCRIPTION     "A tool for working with tensors."
#define CONVERT_DESCRIPTION    "A tool for converting between th-order tensor storage strategies."
#define GENERATE_DESCRIPTION   "A tool for generating varieties of th-order tensors."
#define EFFECTUATE_DESCRIPTION "A tool for performing computations on th-order tensors."
#define VERSION "Version 0.01 (" __DATE__ "), " \
  "Copyright (C) 2011, and GPLv3'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
#define FEEDBACK "Bug reports, feedback, admiration, abuse, etc, to: " \
  "github.com/versionzero"

static char const *map_tool_to_string[] = { 
  "unknown",
  "tensor",
  "convert", 
  "generate",
  "effectuate"
};

static char const *map_tools_to_description[] = { 
  "unknown",
  TENSOR_DESCRIPTION,
  CONVERT_DESCRIPTION,
  GENERATE_DESCRIPTION,
  EFFECTUATE_DESCRIPTION
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

void
print_tool_banner()
{
  message("%s: %s\n\n", tool_name, map_tools_to_description[tool_type]);
  message("%s\n\n", VERSION);
  message("%s\n", FEEDBACK);
}

void
print_tools_with_descriptions(char const *format)
{
  uint i;
  
  for (i = 2; i < COUNT_OF(map_tool_to_string); ++i) {
    message(format, map_tool_to_string[i], map_tools_to_description[i]);
  }
}

void
print_tool_options()
{
  debug("main: cache_size=%d\n", cache_size);
  debug("main: cache_line_size=%d\n", cache_line_size);
  debug("main: emit_latex='%s'\n", bool_to_string(emit_latex));
  debug("main: iterations=%d\n", iterations);
  debug("main: simulate='%s'\n", bool_to_string(simulate));
  debug("main: tracing='%s'\n", bool_to_string(tracing));
  debug("main: verbose='%s'\n", bool_to_string(verbose));
  debug("main: noisiness=%d\n", noisiness);
  debug("main: write_results='%s'\n", bool_to_string(write_results));
}
