
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "operation.h"
#include "strings.h"
#include "tensor.h"
#include "thread.h"
#include "tool.h"
#include "utility.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

cache_t                   *cache;
uint                      cache_size;
uint                      cache_line_size;
uint                      iterations;
uint                      memory_stride;
thread::partition::type_t thread_partition;
uint                      seed;
uint                      thread_count;
char                      *tool_name;
tool::type_t              tool_type;
bool                      tracing;
bool                      simulate;
bool                      human_readable;
bool                      verbose;
verbosity::type_t         noisiness;
bool                      write_results;
bool                      emit_latex;

void
usage()
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s <tool_name> [options] ...\n", tool_name);
  message("\nTools:\n");
  print_tools_with_descriptions("\t* %s\n");
  exit(1);
}

int
main(int argc, char *argv[])
{
  struct {
    void (*main)(int argc, char *argv[]);
  } entrypoints[] = {
    { NULL },
    { NULL },
    { &generate_tool_main },
    { &effectuate_tool_main },
    { NULL }
  };
  
  /* store the our name for future use */
  tool_name = destructive_basename(argv[0]);
  
  /* figure out which tool the user is invoking */
  tool_type = tool_from_string(tool_name);
  
  /* some tools rename our executable--i.e. Condor--so we must guard
     against this here.  That is, if we don't recognize the name, just
     assume it has been renamed. */
  if (tool::unknown == tool_type) {
    tool_type = tool::tensor;
  }
  
  /* if the binary has not been renamed, then the tool name is the
     first parameter in the list.  We handle this very simply: we skip
     over the first element in the argument list and decrement the
     argument count. */
  if (tool::tensor == tool_type) {
    argc--;
    if (0 == argc) {
      usage();
    }
    tool_name = destructive_basename(argv[1]);
    tool_type = tool_from_string(tool_name);
    argv++;
  }
  
  /* overly clever user? */
  if (tool::tensor == tool_type) {
    die("Just had to try, didn't you? No; it's not recursive.\n");
  }
  
  /* typo? */
  if (tool::unknown == tool_type) {
    die("The name '%s' does not match any known tool name.\n", tool_name);
  }
  
  /* set the program's defaults */
  cache           = NULL;
  cache_size      = DEFAULT_CACHE_SIZE;
  cache_line_size = DEFAULT_CACHE_LINE_SIZE;
  emit_latex      = DEFAULT_EMIT_LATEX;
  iterations      = DEFAULT_ITERATIONS;
  human_readable  = DEFAULT_HUMAN_READABLE;
  seed            = DEFAULT_SEED;
  simulate        = DEFAULT_SIMULATE;
  tracing         = DEFAULT_TRACING;
  verbose         = DEFAULT_VERBOSE;
  noisiness       = DEFAULT_VERBOSITY;
  write_results   = DEFAULT_WRITE_RESULTS;
  
  /* run the tool the user requested */
  entrypoints[tool_type].main(argc, argv);
  
  return 0;
}

