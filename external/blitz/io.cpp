#include <blitz/array.h>
#ifdef BZ_HAVE_STD
	#include <fstream>
#else
	#include <fstream.h>
#endif

BZ_USING_NAMESPACE(blitz)

const char* filename = "io.data";
blitz::IndexPlaceholder<0> i;
blitz::IndexPlaceholder<1> j;
blitz::IndexPlaceholder<2> k;

void write_arrays()
{
    ofstream ofs(filename);
    if (ofs.bad())
    {
        cerr << "Unable to write to file: " << filename << endl;
        exit(1);
    }

    Array<float,3> A(3,4,5);
    A = 111 + i + 10 * j + 100 * k;
    ofs << A << endl;

    Array<float,2> B(3,4);
    B = 11 + i + 10 * j;
    ofs << B << endl;

    Array<float,1> C(4);
    C = 1 + i;
    ofs << C << endl;
}

int main()
{
    write_arrays();

    ifstream ifs(filename);
    if (ifs.bad())
    {
        cerr << "Unable to open file: " << filename << endl;
        exit(1);
    }

    Array<float,3> A;
    Array<float,2> B;
    Array<float,1> C;

    ifs >> A >> B >> C;

    cout << "Arrays restored from file: " << A << B << C << endl;

    return 0;
}

