
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

#define OPTION_MESSAGE(x,a,b)    (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)     OPTION_MESSAGE(x, "on", "off")

#define DEFAULT_ITERATIONS       1
#define DEFAULT_OPERATION        operation::n_mode_product
#define DEFAULT_ORIENTATION      orientation::row
#define DEFAULT_SIMULATE         false
#define DEFAULT_STRATEGY         strategy::compressed
#define DEFAULT_VERBOSITY        false
#define DEFAULT_WRITE_RESULTS    false

#define DEFAULT_WORD_SIZE        32
#define DEFAULT_WORD_SIZE_OFFSET 2
#define DEFAULT_CACHE_SIZE       (2*1024)
#define DEFAULT_CACHE_LINE_SIZE  32

cache_t      *cache;
uint         cache_size;
uint         cache_line_size;
uint         iterations;
char         *tool_name;
tool::type_t tool_type;
bool         simulate;
bool         verbose;
bool         write_results;

void
effectuate_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input1> <intput2> ... [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-n\tnumber of times to apply operation (default: %d)\n", DEFAULT_ITERATIONS);
  message("\t-m\tcache size (default: %d)\n", DEFAULT_CACHE_SIZE);
  message("\t-o\toperation (default: %s)\n", operation_to_string(DEFAULT_OPERATION));
  print_operations_with_descriptions("\t\t- %s : %s\n");
  message("\t-s\tsimulate cache (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_SIMULATE));
  message("\t-v\ttoggle verbosity (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
  message("\t-w\twrite results (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_WRITE_RESULTS));
  message("\nExample:\n\n");
  message("\t$ ./tensor %s -o n-mode vector.in tensor.in matrix.out\n", tool_name);
  message("\tReading vector.in ... done [0.000305]\n");
  message("\tReading tensor.in ... done [0.000235]\n");
  message("\tConverting from 'coordinate' to 'compressed-column' ... done [0.000010]\n");
  message("\tWriting matrix.out ... done [0.000031]\n");
  exit(1);
}

vector_t*
timed_vector_read(char const *name)
{
  clock_t  t;
  vector_t *vector;
  
  message("Reading vector %s ... ", name);
  t = clock();
  vector = vector_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));

  return vector;
}

tensor_t*
timed_tensor_read(char const *name)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Reading tensor %s ... ", name);
  t = clock();
  tensor = tensor_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return tensor;
}

void
timed_matrix_write(int argc, char *argv[], int const offset, matrix_t const *matrix)
{
  clock_t  t;
  char     *name;
  FILE     *file;
  
  if (offset == argc) {
    file = stdout;
    message("Writing stdout ... ");
  } else {
    name = argv[offset];
    file = fopen_or_die(name, "w+");
    message("Writing %s ... ", name);
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

/* adapted from: operation_n_mode_product.cc */
void
calculate_matrix_dimentions(tensor_t const *tensor, uint *m, uint *n)
{
  *m = 0;
  *n = 0;
  switch (tensor->orientation) {
  case orientation::row:
    /* rows * tubes */
    *m = tensor->m;
    *n = tensor->l;
    break;
  case orientation::column:
    /* columns * tubes */
    *m = tensor->n;
    *n = tensor->l;
    break;
  case orientation::tube:
    /* rows * columns */
    *m = tensor->m;
    *n = tensor->n;
    break;
  default:
    die("calculate_matrix_dimentions: unknown or unsupported orientation %d \n", tensor->orientation);
    break;
  }
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
  
  calculate_matrix_dimentions(tensor, &m, &n);
  matrix = matrix_malloc(m, n);
  
  cache = NULL;
  if (simulate) {
    cache = cache_malloc(cache_size, tensor->nnz*5);
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
  cache_size    = DEFAULT_CACHE_SIZE;
  iterations    = DEFAULT_ITERATIONS;
  operation     = DEFAULT_OPERATION;
  simulate      = DEFAULT_SIMULATE;
  verbose       = DEFAULT_VERBOSITY;
  write_results = DEFAULT_WRITE_RESULTS;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, "hm:n:o:svw"))) {
    switch (c) {
    case 'h': 
      effectuate_tool_usage();
      break;
    case 'm':
      cache_size = atoi(optarg);
      if (0 == cache_size) {
	cache_size = DEFAULT_CACHE_SIZE;
      }
      debug("effectuate: cache_size=%d\n", cache_size);
      break;
    case 'n':
      iterations = atoi(optarg);
      if (0 == iterations) {
	iterations = DEFAULT_ITERATIONS;
      }
      debug("effectuate: iterations=%d\n", iterations);
      break;
    case 'o': 
      if (isdigit(optarg[0])) {
	operation = (operation::type_t) atoi(optarg);
      } else {
	operation = string_to_operation(optarg);
      }
      debug("effectuate: operation='%s'\n", operation_to_string(operation));
      break;
    case 's':
      simulate = !simulate;
      debug("effectuate: simulate='%s'\n", bool_to_string(simulate));
      break;
    case 'v': 
      verbose = !verbose;
      debug("effectuate: verbose='%s'\n", bool_to_string(verbose));
      break;
    case 'w':
      write_results = !write_results;
      debug("effectuate: write_results='%s'\n", bool_to_string(write_results));
      break;
    case '?':
      if (isprint(optopt)) {
	die("Unknown option `-%c'.\n", optopt);
      } else {
	die("Unknown option character `\\x%x'.\n", optopt);
      }
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

void
convert_tool_usage() 
{
  print_tool_banner();
  message("\nUsage:\n");
  message("\t%s [options] <input> [output]\n", tool_name);
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-s\tstrategy (default: %s)\n", strategy_to_string(DEFAULT_STRATEGY));
  print_strategies("\t\t- %s\n");
  message("\t-o\torientation (default: %s)\n", orientation_to_string(DEFAULT_ORIENTATION));
  print_orientations("\t\t- %s\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
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
  clock_t  t;
  tensor_t *tensor;
  
  message("Converting from '%s' to '%s-%s' ... ", 
	  strategy_to_string(source->strategy), 
	  strategy_to_string(strategy),
	  orientation_to_string(orientation));
  t = clock();
  tensor = tensor_convert(source, strategy, orientation);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return tensor;
}

void
convert_tool_main(int argc, char *argv[])
{
  int                 c, offset;
  clock_t             t;
  char                *name;
  tensor_t            *tensor, *result;
  FILE                *file;
  strategy::type_t    strategy;
  orientation::type_t orientation;
  
  /* just to be safe, set the tensors to null */
  tensor = result = NULL;
  
  /* set the program's defaults */
  orientation = DEFAULT_ORIENTATION;
  strategy    = DEFAULT_STRATEGY;
  verbose     = DEFAULT_VERBOSITY;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, "ho:s:v"))) {
    switch (c) {
    case 'h': 
      convert_tool_usage();
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
    case '?':
      if (isprint(optopt)) {
	die("Unknown option `-%c'.\n", optopt);
      } else {
	die("Unknown option character `\\x%x'.\n", optopt);
      }
      break;
    default:
      abort();
      break;
    }    
  }
  
  /* count the number of remaining arguments */
  if (argc-optind < 1) {
    convert_tool_usage();
  }
  
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
  
  if (offset == argc) {
    file = stdout;
    message("Writing stdout ... ");
  } else {
    name = argv[offset++];
    file = fopen_or_die(name, "w+");
    message("Writing %s ... ", name);
  }  
  t = clock();
  tensor_fwrite(file, result);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  tensor_free(result);
  tensor_free(tensor);
}

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
    { &convert_tool_main },
    { &effectuate_tool_main },
    { NULL }
  };
    
  /* store the our name for future use */
  tool_name = mybasename(argv[0]);
  
  /* figure out which tool the user is invoking */
  tool_type = tool_from_string(tool_name);
  
  /* if the binary has not been renamed, then the tool name is the
     first parameter in the list.  We handle this very simply: we skip
     over the first element in the argument list and decrement the
     argument count. */
  if (tool::tensor == tool_type) {
    argc--;
    if (0 == argc) {
      usage();
    }
    tool_name = mybasename(argv[1]);
    tool_type = tool_from_string(tool_name);
    argv++;
  }
  
  /* overly clever user? */
  if (tool::tensor == tool_type) {
    die("Just had to try, didn't you? No, it's not recursive.\n");
  }
  
  /* typo? */
  if (tool::unknown == tool_type) {
    die("Using this binary as the '%s' tool is not currently supported.\n", tool_name);
  }
  
  /* run the tool the user requested */
  entrypoints[tool_type].main(argc, argv);
  
  return 0;
}

