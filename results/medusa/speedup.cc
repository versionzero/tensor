
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

int
main(int argc, char *argv[])
{
  int i, k, iterations = 10;
  double d, speedup;
  vector<double> v, m;
  if (argc > 1) {
    iterations = atoi(argv[1]);
  }
  for (int j = 0; j < 6; ++j) {
    v.clear();
    cin >> k;
    for (int i = 0; i < 10; ++i) {
      cin >> d;
      v.push_back(d);
    }
    m.push_back(accumulate(v.begin(), v.end(), 0.0)/v.size());
  }
  for (i = 0; i < 6; ++i) {
    speedup = m[0]/m[i];
    k       = (1 << i);
    cout << k << " " << speedup << endl;
    cerr << k << " " << speedup/k << endl;
  }
  return 0;
}
