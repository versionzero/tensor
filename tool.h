
#ifndef _TOOL_H_
#define _TOOL_H_

#include "types.h"

namespace tool {
  typedef enum {
    unknown,
    tensor,
    convert,
    effectuate
  } type_t;
}

tool::type_t tool_from_string(char const *s);
char const* string_from_tool(tool::type_t tool);

void print_tool_banner();
void print_tools_with_descriptions(char const *format);
 
#endif /* _UTILITY_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
