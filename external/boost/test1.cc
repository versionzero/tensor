#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

int main () 
{
    using namespace boost::numeric::ublas;
    compressed_matrix<double> m (3, 3, 3 * 3);
    int k = 0;
    for (unsigned i = 0; i < 2; ++ i)
        for (unsigned j = 0; j < m.size2 (); ++ j)
            m (i, j) = k++;
    std::cout << m << std::endl;
}
