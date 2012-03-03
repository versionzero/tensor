
#include <cstdio>
#include <iostream>

#include "mmio.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

namespace io    = boost::iostreams;
namespace ublas = boost::numeric::ublas;

using std::cout;
using std::endl;

int
main(int argc, char *argv[])
{
  /* need a matrix file as input */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s filename\n", argv[0]);
    exit(1);
  }
  
  /* filter out matrix market comments */
  io::filtering_istream fin;
  fin.push(mmio_comments_filter());
  fin.push(io::file_source(argv[1]));
  
  /* read in the input matrix */
  ublas::compressed_matrix<double> matrix;
  fin >> matrix;
  
  
  
  
  cout << matrix << endl;
  
  return 0;
}
