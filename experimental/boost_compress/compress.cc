
#include <cstdio>
#include <iostream>
#include <vector>

#include "mmio.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

static int w = 1;
static int z = 2;

void
random_seed(int seed)
{
  if (seed > 0) {
    w = seed + 1;
    z = w + w + 1;
  }
}

/* http://en.wikipedia.org/wiki/Random_number_generator */
int
random_marsaglia()
{
  z = 36969 * (z & 65535) + (z >> 16);
  w = 18000 * (w & 65535) + (w >> 16);
  return (z << 16) + w;  /* 32-bit result */
}

int 
random_between(int min, int max)
{
  return random_marsaglia() % (max-min+1) + min;
}

bool
picked(int n, int k)
{
  return (random_between(0, n) == k);
}

using std::cout;
using std::endl;

namespace io    = boost::iostreams;
namespace ublas = boost::numeric::ublas;

typedef ublas::compressed_matrix<double> compressed_matrix;
typedef std::vector<compressed_matrix>   compressed_tensor;

int
main(int argc, char *argv[])
{
  /* need a matrix file as input */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s filename [seed]\n", argv[0]);
    exit(1);
  }
  
  /* let the user pick a random seed to start from */
  random_seed((argc > 2 ? atoi(argv[2]) : 0));
  
  /* filter matrix-market comments */
  io::filtering_istream fin;
  fin.push(mmio_comments_filter());
  fin.push(io::file_source(argv[1]));
  
  /* read in the input (seed) matrix */
  ublas::compressed_matrix<double> matrix;
  fin >> matrix;
  
  /* print the input matrix */
  cout << "Input matrix: " << matrix << endl;
  
  /* create our tensor based on the input matrix--we assume, for now,
     and for simplicity, that the input matricies are cubic in
     structure */
  int n = matrix.size1();
  compressed_tensor tensor(n, compressed_matrix(n, n));
  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
	if (matrix(i, j) > 0.0 && picked(n, 0)) {
	  tensor[k](i, j) = 1.0;
	}
      }
    }
  }
  
  /* print the tensor out */
  for (int k = 0; k < n; ++k) {
    cout << tensor[k] << endl;
  }
  
  return 0;
}
