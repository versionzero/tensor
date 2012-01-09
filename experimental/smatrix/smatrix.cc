
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

void
test_mapped_matrix()
{
  using namespace boost::numeric::ublas;
  mapped_matrix<double> m (3, 3, 3 * 3);
  for (unsigned i = 0; i < m.size1 (); ++ i)
    for (unsigned j = 0; j < m.size2 (); ++ j)
      m (i, j) = 3 * i + j;
  std::cout << m << std::endl;
}

void
test_compressed_matrix()
{
  using namespace boost::numeric::ublas;
  compressed_matrix<double> m (3, 3, 3 * 3);
  for (unsigned i = 0; i < m.size1 (); ++ i)
    for (unsigned j = 0; j < m.size2 (); ++ j)
      m (i, j) = 3 * i + j;
  std::cout << m << std::endl;
}

int main () {
  test_mapped_matrix();
  test_compressed_matrix();
}
