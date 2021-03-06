
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "operation.h"
#include "tensor.h"
#include "tool.h"
#include "utility.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

extern cache_t           *cache;
extern uint              cache_size;
extern uint              cache_line_size;
extern bool              emit_latex;
extern uint              iterations;
extern char              *tool_name;
extern tool::type_t      tool_type;
extern bool              simulate;
extern bool              verbose;
extern verbosity::type_t noisiness;
extern bool              write_results;

void
convert_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input> [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-l\temit LaTeX code as output (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_EMIT_LATEX));
  message("\t-s\tstrategy (default: %s)\n", strategy_to_string(DEFAULT_STRATEGY));
  print_strategies("\t\t- %s\n");
  message("\t-o\torientation (default: %s)\n", orientation_to_string(DEFAULT_ORIENTATION));
  print_orientations("\t\t- %s\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_VERBOSE));
  message("\t-V\tdebug verbosity level (default: %d/%d)\n", DEFAULT_VERBOSITY, verbosity::max);
  message("\nExample:\n\n");
  message("\t$ ./tensor %s -s compressed -o column ieee-fig4.in tensor.out\n", tool_name);
  message("\tReading ieee-fig4.in ... done [0.000305]\n");
  message("\tConverting from 'coordinate' to 'compressed-column' ... done [0.000010]\n");
  message("\tWriting tensor.out ... done [0.000031]\n");
  exit(1);
}

tensor_t*
timed_tensor_convert(tensor_t *source, strategy::type_t strategy, orientation::type_t orientation)
{
  precision_timer_t  t;
  tensor_t *tensor;
  
  progress("Converting from '%s' to '%s-%s' ... ",
	  strategy_to_string(source->strategy),
	  strategy_to_string(strategy),
	  orientation_to_string(orientation));
  timer_start(&t);
  tensor = tensor_convert(source, strategy, orientation);
  timer_end(&t);
  print_elapsed_time(t);
  
  return tensor;
}

void
convert_tool_main(int argc, char *argv[])
{
  int                 c, offset;
  char                *name;
  tensor_t            *tensor, *result;
  strategy::type_t    strategy;
  orientation::type_t orientation;
  
  /* just to be safe, set the tensors to null */
  tensor = result = NULL;
  
  /* set the program's defaults */
  orientation = DEFAULT_ORIENTATION;
  strategy    = DEFAULT_STRATEGY;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, ":hlo:s:vV:"))) {
    switch (c) {
    case 'h': 
      convert_tool_usage();
      break;
    case 'l':
      emit_latex = !emit_latex;
      break;
    case 'o':
      if (isdigit(optarg[0])) {
	orientation = (orientation::type_t) atoi(optarg);
      } else {
	orientation = string_to_orientation(optarg); 
      }
      break;
    case 's':
      if (isdigit(optarg[0])) {
	strategy = (strategy::type_t) atoi(optarg);
      } else {
	strategy = string_to_strategy(optarg);
      }
      break;
    case 'v': 
      verbose = !verbose;
      break;
    case 'V':
      noisiness = (verbosity::type_t) atoi(optarg);
      if (0 == noisiness) {
	noisiness = DEFAULT_VERBOSITY;
      }
      break;
    case ':':
      die("Option -%c requires an operand; that is, an integer or string value.\n", optopt);
      break;
    case '?':
      die("Unknown option: `-%c'\n", optopt);
      break;
    default:
      abort();
      break;
    }
  }
  
  if (noisiness > DEFAULT_VERBOSITY) {
    verbose = true;
  }
  
  /* count the number of remaining arguments */
  if (argc-optind < 1) {
    convert_tool_usage();
  }
  
  /* print program options, for debugging purposes */
  print_tool_options();
  debug("convert_tool_main: orientation='%s'\n", orientation_to_string(orientation));
  debug("convert_tool_main: strategy='%s'\n", strategy_to_string(strategy));
  
  /* parse the remaining command line options */
  offset = optind;
  name   = argv[offset++];
  tensor = timed_tensor_read(name);
  debug("main: tensor=0x%x\n", tensor);
  
  if (strategy == tensor->strategy) {
    /* we'll deal with differing orientation when it comes up */
    result = tensor;
    tensor = NULL;
  } else {
    result = timed_tensor_convert(tensor, strategy, orientation);
  }
  debug("main: result=0x%x\n", result);
  
  timed_tensor_write(argc, argv, offset, result);
  
  tensor_free(result);
  tensor_free(tensor);
}
