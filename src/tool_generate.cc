
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "generate.h"
#include "matrix.h"
#include "operation.h"
#include "random.h"
#include "tensor.h"
#include "tool.h"
#include "utility.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <ctype.h>
#include <unistd.h>

extern cache_t           *cache;
extern uint              cache_size;
extern uint              cache_line_size;
extern uint              iterations;
extern uint              seed;
extern char              *tool_name;
extern tool::type_t      tool_type;
extern bool              simulate;
extern bool              verbose;
extern verbosity::type_t noisiness;
extern bool              write_results;

void
generate_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input> [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-s\trandom number generator seed (default: %d)\n", DEFAULT_SEED);
  exit(1);
}

void
timed_generation(int argc, char *argv[])
{
  int      offset;
  char     *name;
  matrix_t *matrix;
  tensor_t *tensor;
  
  /* just to be safe, set the matricies and tensors to null */
  matrix = NULL;
  tensor = NULL;
  
  offset = optind;
  name   = argv[offset++];
  matrix = timed_matrix_read(name);
  debug("timed_generation: matrix=0x%x\n", matrix);
  
  tensor = generate_tensor_from_matrix(matrix);
  debug("timed_generation: tensor=0x%x\n", tensor);
  
  timed_tensor_write(argc, argv, offset, tensor);
  
  matrix_free(matrix);
  tensor_free(tensor);
}

void
generate_tool_main(int argc, char *argv[])
{
  int  c;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, ":hvV:"))) {
    switch (c) {
    case 'h': 
      generate_tool_usage();
      break;
    case 's':
      seed = atoi(optarg);
      if (0 == seed) {
	seed = DEFAULT_SEED;
      }
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
    generate_tool_usage();
  }
  
  /* print program options, for debugging purposes */
  print_tool_options();
  debug("generate_tool_main: seed=%d\n", seed);
    
  /* seed the random number generator */
  random_seed(seed);
  
  /* pass control over to some naive timing procedures */
  timed_generation(argc, argv);
}

