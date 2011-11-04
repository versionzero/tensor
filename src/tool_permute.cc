
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "memory.h"
#include "operation.h"
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

extern bool              human_readable;
extern char              *tool_name;
extern tool::type_t      tool_type;
extern bool              tracing;
extern bool              verbose;
extern verbosity::type_t noisiness;
extern bool              write_results;

static permutation_heuristic::type_t heuristic;

void
permute_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input1> <intput2> ... [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-p\tpermutation heuristic (default: %s)\n", permutation_heuristic_to_string(DEFAULT_PERMUTATION_HEURISTIC));
  print_permutation_heuristics_with_descriptions("\t\t- %s : %s\n");
  message("\t-t\ttoggle tracing (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_TRACING));
  message("\t-v\ttoggle verbosity (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_VERBOSE));
  message("\t-V\tdebug verbosity level (default: %d/%d)\n", DEFAULT_VERBOSITY, verbosity::max);
  message("\t-w\twrite results (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_WRITE_RESULTS));
  message("\nExample:\n\n");
  message("\t$ ./tensor %s -p naive-minimum tensor.in tensor.out\n", tool_name);
  message("\tReading vector.in ... done [0.000305]\n");
  message("\tReading tensor.in ... done [0.000235]\n");
  message("\tConverting from 'coordinate' to 'compressed-column' ... done [0.000010]\n");
  message("\tWriting matrix.out ... done [0.000031]\n");
  exit(1);
}

tensor_t*
timed_tensor_permute(tensor_t *tensor)
{
  precision_timer_t  t;
  tensor_t *permuted;
  
  progress("Permuting tensor using the '%s' heuristic ... ", 
	   permutation_heuristic_to_string(heuristic));
  timer_start(&t);
  permuted = tensor_permute(tensor, heuristic);
  timer_end(&t);
  print_elapsed_time(t);
  
  return permuted;
}

void
timed_permutation(int argc, char *argv[])
{
  int      offset;
  char     *name;
  tensor_t *tensor, *permuted;
  
  offset = optind;
  name   = argv[offset++];
  tensor = timed_tensor_read(name);
  debug("timed_permutation: tensor=0x%x\n", tensor);
  
  if (permutation_heuristic::none == heuristic) {
    print_elapsed_time(0.0); /* just a no-op */
  } else {
    permuted = timed_tensor_permute(tensor);
    tensor_free(tensor);
    tensor = permuted;
  }
  debug("timed_permutation: permutation=0x%x\n", tensor);
  
  /* if we are not printing times for each procedure out in a human
     consumable way, then we need to terminate the line containing all
     the timings for this instance */
  if (!human_readable) {
    message("\n");
  }
  
  timed_tensor_write(argc, argv, offset, tensor);
  tensor_free(tensor);
}

void
permute_tool_main(int argc, char *argv[])
{
  int c;
  
  /* set the program's defaults */
  heuristic = DEFAULT_PERMUTATION_HEURISTIC;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, ":hp:tuvV:w"))) {
    switch (c) {
    case 'h': 
      permute_tool_usage();
      break;
    case 'p': 
      if (isdigit(optarg[0])) {
	heuristic = (permutation_heuristic::type_t) atoi(optarg);
      } else {
	heuristic = string_to_permutation_heuristic(optarg);
      }
      break;
    case 't':
      tracing = !tracing;
      break;
    case 'u':
      human_readable = !human_readable;
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
    case 'w':
      write_results = !write_results;
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
    permute_tool_usage();
  }
  
  /* print program options, for debugging purposes */
  print_tool_options();
  debug("permute_tool_main: heuristic='%s'\n", permutation_heuristic_to_string(heuristic));
  
  /* pass control over to some naive timing procedures */
  timed_permutation(argc, argv);
}
