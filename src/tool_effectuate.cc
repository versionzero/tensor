
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "memory.h"
#include "operation.h"
#include "tensor.h"
#include "thread.h"
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

extern association::type_t       operand_association;
extern cache_t			 *cache;
extern uint			 cache_size;
extern uint			 cache_line_size;
extern bool			 human_readable;
extern uint			 iterations;
extern uint			 memory_stride;
extern uint			 thread_count;
extern thread::partition::type_t thread_partition;
extern char			 *tool_name;
extern tool::type_t		 tool_type;
extern bool			 simulate;
extern bool			 tracing;
extern bool			 verbose;
extern verbosity::type_t	 noisiness;
extern bool			 write_results;

static operation::type_t optcode;

void
effectuate_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input1> <intput2> ... [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
#if !defined (NOSIMULATE)
  message("\t-l\tcache line size (default: %d)\n", DEFAULT_CACHE_LINE_SIZE);
  message("\t-m\tcache size (default: %d)\n", DEFAULT_CACHE_SIZE);
#endif
  message("\t-n\tnumber of times to apply operation (default: %d)\n", DEFAULT_ITERATIONS);
  message("\t-o\toperation (default: %s)\n", operation_to_string(DEFAULT_OPERATION));
  print_operations_with_descriptions("\t\t- %s : %s\n");
#if !defined (NOSIMULATE)
  message("\t-s\tsimulate cache (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_SIMULATE));
#endif
  message("\t-t\tnumer of thread_count to use (default: %d)\n", DEFAULT_THREAD_COUNT);
  message("\t-T\ttoggle tracing (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_TRACING));
  message("\t-v\ttoggle verbosity (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_VERBOSE));
  message("\t-V\tdebug verbosity level (default: %d/%d)\n", DEFAULT_VERBOSITY, verbosity::max);
  message("\t-w\twrite results (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_WRITE_RESULTS));
  message("\nExample:\n\n");
  message("\t$ ./tensor %s -o n-mode vector.in tensor.in matrix.out\n", tool_name);
  message("\tReading vector.in ... done [0.000305]\n");
  message("\tReading tensor.in ... done [0.000235]\n");
  message("\tConverting from 'coordinate' to 'compressed-column' ... done [0.000010]\n");
  message("\tWriting matrix.out ... done [0.000031]\n");
  exit(1);
}

void
timed_matrix_write(int argc, char *argv[], int const offset, matrix_t const *matrix)
{
  precision_timer_t  t;
  char     *name;
  FILE     *file;
  
  if (offset == argc) {
    file = stdout;
    progress("Writing matrix to stdout ... ");
  } else {
    name = argv[offset];
    file = fopen_or_die(name, "w+");
    progress("Writing matrix to %s ... ", name);
  }
  
  timer_start(&t);
  matrix_fwrite(file, matrix, format::array);
  timer_end(&t);
  print_elapsed_time(t);
  
  if (stdout != file) {
    fclose(file);
  }
}

void
timed_operation_n_mode_product(matrix_t *matrix, vector_t *vector, tensor_t *tensor)
{
  precision_timer_t  t;
  
  matrix_clear(matrix);
  progress("Performing operation '%s' ... ", 
	   operation_to_description_string(operation::n_mode_product));
  timer_start(&t);
  operation_n_mode_product(matrix, vector, tensor);
  timer_end(&t);
  print_elapsed_time(t);
}

void
timed_operation_n_mode_product(int argc, char *argv[])
{
  uint     i;
  int      offset;
  char     *name;
  vector_t *vector;
  matrix_t *matrix;
  tensor_t *tensor;
  
  offset = optind;
  name   = argv[offset++];
  vector = timed_vector_read(name);
  debug("timed_operation_n_mode_product: vector=0x%x\n", vector);
  
  name   = argv[offset++];
  tensor = timed_tensor_read(name);
  debug("timed_operation_n_mode_product: tensor=0x%x\n", tensor);
  
  compatible(vector, tensor);
  matrix = matrix_malloc(tensor->m, tensor->n);
  debug("timed_operation_n_mode_product: matrix=0x%x\n", matrix);
  
  cache = NULL;
  if (simulate) {
    cache = cache_malloc(cache_size, cache_line_size);
    cache_supported(cache);
  }
  
  for (i = 0; i < iterations; ++i) {
    timed_operation_n_mode_product(matrix, vector, tensor);
  }
  
  if (write_results) {
    timed_matrix_write(argc, argv, offset, matrix);
  }
  
  /* if we are not printing times for each procedure out in a human
     consumable way, then we need to terminate the line containing all
     the timings for this instance */
  if (!human_readable) {
    message("\n");
  }
  
  if (simulate) {
    cache_print_profile(cache);
    cache_free(cache);
  }
  
  vector_free(vector);
  matrix_free(matrix);
  tensor_free(tensor);
}

void
timed_operation(int argc, char *argv[])
{
  switch (optcode) {
  case operation::n_mode_product:
    timed_operation_n_mode_product(argc, argv);
    break;
  default:
    die("Operation '%d' not currently supported.\n", optcode);
    break;
  }
}

void
effectuate_tool_main(int argc, char *argv[])
{
  int c;
  
  /* set the program's defaults */
  operand_association = DEFAULT_ASSOCIATION;
  memory_stride       = DEFAULT_MEMORY_STRIDE;
  optcode             = DEFAULT_OPERATION;
  thread_count        = DEFAULT_THREAD_COUNT;
  thread_partition    = DEFAULT_THREAD_PARTITION;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, ":a:hl:m:n:o:p:r:st:TuvV:w"))) {
    switch (c) {
    case 'a':
      if (isdigit(optarg[0])) {
	operand_association = (association::type_t) atoi(optarg);
      } else {
	operand_association = string_to_association(optarg);
      }
      break;
    case 'h': 
      effectuate_tool_usage();
      break;
    case 'l':
      cache_line_size = atoi(optarg);
      if (0 == cache_line_size) {
	cache_line_size = DEFAULT_CACHE_LINE_SIZE;
      }
      break;
    case 'm':
      cache_size = atoi(optarg);
      if (0 == cache_size) {
	cache_size = DEFAULT_CACHE_SIZE;
      }
      break;
    case 'n':
      iterations = atoi(optarg);
      if (0 == iterations) {
	iterations = DEFAULT_ITERATIONS;
      }
      break;
    case 'o': 
      if (isdigit(optarg[0])) {
	optcode = (operation::type_t) atoi(optarg);
      } else {
	optcode = string_to_operation(optarg);
      }
      break;
    case 'p':
      if (isdigit(optarg[0])) {
	thread_partition = (thread::partition::type_t) atoi(optarg);
      } else {
	thread_partition = string_to_thread_partition(optarg);
      }
      break;
    case 'r':
      memory_stride = atoi(optarg);
      if (0 == memory_stride) {
	memory_stride = DEFAULT_MEMORY_STRIDE;
      }
      break;
    case 's':
      simulate = !simulate;
      break;
    case 't':
      thread_count = atoi(optarg);
      if (0 == thread_count) {
	thread_count = DEFAULT_THREAD_COUNT;
      }
      break;
    case 'T':
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
  if (argc-optind < 2) {
    effectuate_tool_usage();
  }
  
  /* print program options, for debugging purposes */
  print_tool_options();
  debug("effectuate_tool_main: operand_association='%s'\n", association_to_string(operand_association));
  debug("effectuate_tool_main: operation='%s'\n", operation_to_string(optcode));
  debug("effectuate_tool_main: memory_stride=%d\n", memory_stride);
  debug("effectuate_tool_main: thread_count=%d\n", thread_count);
  debug("effectuate_tool_main: thread_partition='%s'\n", thread_partition_to_string(thread_partition));
  
  /* if we are just running a simulation, then we only do one
     iteration; otherwise, it would be really slow */
  if (simulate) {
    iterations = 1;
  }
  
  /* pass control over to some naive timing procedures */
  timed_operation(argc, argv);
}
