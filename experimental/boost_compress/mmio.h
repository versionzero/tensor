/* 
*   Matrix Market I/O library for C++
*/

#ifndef _MM_IO_H_
#define _MM_IO_H_

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/stdio.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

class mmio_comments_filter : public boost::iostreams::stdio_filter {
public:
  
  explicit
  mmio_comments_filter(char comment = '%') 
    : comment_(comment) 
  {}
  
private:
  
  char comment_;
  
  void 
  do_filter() {
    int  c;
    bool skip = false;
    while ((c = std::cin.get()) != EOF) {
      skip = (c == comment_ ? true : c == '\n' ? false : skip);
      if (!skip) {
	std::cout.put(c);
      }
    }
  }
  
};

template<class Matrix>
std::istream& operator>>(std::istream &in, Matrix &matrix) {
  
  int m, n, nnz;
  in >> m >> n >> nnz;
  matrix.resize(m, n, false);
  matrix.reserve(nnz, false);
  
  int    i, j;
  double value;
  while (nnz--) {
    in >> i >> j >> value;
    matrix(i-1, j-1) = value;   /* adjust from 1-based to 0-based */
  }
  
  return in;
}

#endif
