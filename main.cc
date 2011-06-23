
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

#define CONVERT_DESCRIPTION "A tool for converting between th-order tensor storage strategies."
#define EFFECTUATE_DESCRIPTION "A tool for performing computations on th-order tensors."
#define VERSION     "Version 0.01 (" __DATE__ "), " \
  "Copyright (C) 2011, and GPLv3'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

#define DEFAULT_VERBOSITY   false
#define DEFAULT_ORIENTATION orientation::row
#define DEFAULT_STRATEGY    strategy::compressed
#define DEFAULT_OPERATION   operation::n_mode_product

int  storage_strategy;
int  operation_code;
char *tool_name;
bool verbose;

void
effectuate_tool_usage() 
{
  message("%s: %s\n\n", tool_name, EFFECTUATE_DESCRIPTION);
  message("%s\n", VERSION);
  message("\nUsage:\n");
  message("\t%s [options] <input1> <intput2> ... [output]\n",
	  tool_name);  
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-o\toperation (default: %s)\n", 
	  operation_to_string(DEFAULT_OPERATION));
  print_operations_with_descriptions("\t\t- %s : %s\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
  message("\nExample:\n\n");
  message("\t$ ./%s -o pt -o vector.in tensor.in matrix.out\n", tool_name);
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

matrix_t*
timed_operation_n_mode_product(vector_t *vector, tensor_t *tensor)
{
  clock_t  t;
  matrix_t *matrix;
  
  message("Performing operation '%s' ... ", operation_to_description_string(operation::n_mode_product));
  t = clock();
  matrix = operation_n_mode_product(vector, tensor);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return matrix;
}

void
operation_n_mode_product(int argc, char *argv[])
{
  int      offset;
  char     *name;
  vector_t *vector;
  matrix_t *matrix;
  tensor_t *tensor;
  
  offset = optind;
  name   = argv[offset++];
  vector = timed_vector_read(name);
  debug("operation_n_mode_product: vector=0x%x\n", vector);
  
  name   = argv[offset++];
  tensor = timed_tensor_read(name);
  debug("operation_n_mode_product: tensor=0x%x\n", tensor);
  
  matrix = timed_operation_n_mode_product(vector, tensor);
  debug("operation_n_mode_product: matrix=0x%x\n", matrix);
  
  timed_matrix_write(argc, argv, offset, matrix);
  
  vector_free(vector);
  matrix_free(matrix);
  tensor_free(tensor);
}

void
timed_operation(operation::type_t operation, int argc, char *argv[])
{
  switch (operation) {
  case operation::n_mode_product:
    operation_n_mode_product(argc, argv);
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
  verbose   = false;
  operation = DEFAULT_OPERATION;
  
  /* we will privide our own error messages */
  opterr = 0;
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, "ho:v"))) {
    switch (c) {
    case 'h': 
      effectuate_tool_usage();
      break;
    case 'o': 
      if (isdigit(optarg[0])) {
	operation = (operation::type_t) atoi(optarg);
      } else {
	operation = string_to_operation(optarg);
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
  if (argc-optind < 2) {
    effectuate_tool_usage();
  }
  
  /* pass control over to some naive timing procedures */
  timed_operation(operation, argc, argv);
}

void
convert_tool_usage() 
{
  message("%s: %s\n\n", tool_name, CONVERT_DESCRIPTION);
  message("%s\n", VERSION);
  message("\nUsage:\n");
  message("\t%s [options] <input> [output]\n",
	  tool_name);  
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-s\tstrategy (default: %s)\n", 
	  strategy_to_string(DEFAULT_STRATEGY));
  print_strategies("\t\t- %s\n");
  message("\t-o\torientation (default: %s)\n", 
	  orientation_to_string(DEFAULT_ORIENTATION));
  print_orientations("\t\t- %s\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
  message("\nExample:\n\n");
  message("\t$ ./%s -s compressed -o column ieee-fig4.in tensor.out\n", tool_name);
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
  verbose     = false;
  orientation = DEFAULT_ORIENTATION;
  strategy    = DEFAULT_STRATEGY;
  
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

int
main(int argc, char *argv[])
{
  tool::type_t tool;
  struct {
    void (*main)(int argc, char *argv[]);
  } entrypoints[] = {
    { NULL },
    { &convert_tool_main },
    { &effectuate_tool_main },
    { NULL }
  };
    
  /* store the our name for future use */
  tool_name = basename(argv[0]);
  
  /* figure out which tool the user is invoking */
  tool = tool_from_string(tool_name);
  
  if (tool::unknown == tool) {
    die("Using this binary as the '%s' tool is not currently supported.\n", tool_name);
  }
  
  /* run the tool the user requested */
  entrypoints[tool].main(argc, argv);
  
  return 0;
}

