
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
extern uint              iterations;
extern char              *tool_name;
extern tool::type_t      tool_type;
extern bool              simulate;
extern bool              verbose;
extern verbosity::type_t noisiness;
extern bool              write_results;

void
effectuate_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input1> <intput2> ... [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-l\tcache line size (default: %d)\n", DEFAULT_CACHE_LINE_SIZE);
  message("\t-m\tcache size (default: %d)\n", DEFAULT_CACHE_SIZE);
  message("\t-n\tnumber of times to apply operation (default: %d)\n", DEFAULT_ITERATIONS);
  message("\t-o\toperation (default: %s)\n", operation_to_string(DEFAULT_OPERATION));
  print_operations_with_descriptions("\t\t- %s : %s\n");
  message("\t-s\tsimulate cache (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_SIMULATE));
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
  clock_t  t;
  char     *name;
  FILE     *file;
  
  if (offset == argc) {
    file = stdout;
    message("Writing matrix to stdout ... ");
  } else {
    name = argv[offset];
    file = fopen_or_die(name, "w+");
    message("Writing matrix to %s ... ", name);
  }
  
  t = clock();
  matrix_fwrite(file, matrix, format::coordinate);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  if (stdout != file) {
    fclose(file);
  }
}

void
timed_operation_n_mode_product(matrix_t *matrix, vector_t *vector, tensor_t *tensor)
{
  clock_t  t;
  
  message("Performing operation '%s' ... ", operation_to_description_string(operation::n_mode_product));
  t = clock();
  operation_n_mode_product_inplace(matrix, vector, tensor);
  message("done [%lf]\n", SECONDS_SINCE(t));
}

void
timed_operation_n_mode_product(int argc, char *argv[])
{
  uint     i, m, n;
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
  calculate_output_matrix_dimentions(tensor, &m, &n);
  matrix = matrix_malloc(m, n);
  
  cache = NULL;
  if (simulate) {
    cache = cache_malloc(cache_size, cache_line_size);
    cache_supported(cache);
  }
  
  for (i = 0; i < iterations; ++i) {
    timed_operation_n_mode_product(matrix, vector, tensor);
  }
  debug("timed_operation_n_mode_product: matrix=0x%x\n", matrix);
  
  if (write_results) {
    timed_matrix_write(argc, argv, offset, matrix);
  }
  
  if (simulate) {
    cache_print_statistics(cache);
    cache_free(cache);
  }
  
  vector_free(vector);
  matrix_free(matrix);
  tensor_free(tensor);
}

void
timed_operation(operation::type_t operation, int argc, char *argv[])
{
  switch (operation) {
  case operation::n_mode_product:
    timed_operation_n_mode_product(argc, argv);
    break;
  default:
    die("Operation '%d' not currently supported.\n", operation);
    break;
  }
}

void
effectuate_tool_main(int argc, char *argv[])
{
  int               c;
  operation::type_t operation;
  
  /* set the program's defaults */
  cache_size      = DEFAULT_CACHE_SIZE;
  cache_line_size = DEFAULT_CACHE_LINE_SIZE;
  iterations      = DEFAULT_ITERATIONS;
  operation       = DEFAULT_OPERATION;
  simulate        = DEFAULT_SIMULATE;
  verbose         = DEFAULT_VERBOSE;
  noisiness       = DEFAULT_VERBOSITY;
  write_results   = DEFAULT_WRITE_RESULTS;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, ":hl:m:n:o:svV:w"))) {
    switch (c) {
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
	operation = (operation::type_t) atoi(optarg);
      } else {
	operation = string_to_operation(optarg);
      }
      break;
    case 's':
      simulate = !simulate;
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
  
  /* count the number of remaining arguments */
  if (argc-optind < 2) {
    effectuate_tool_usage();
  }
  
  /* if we are just running a simulation, then we only do one
     iteration; otherwise, it would be really slow */
  if (simulate) {
    iterations = 1;
  }
  
  /* pass control over to some naive timing procedures */
  timed_operation(operation, argc, argv);
}

